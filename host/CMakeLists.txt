set(TARGET host)
set(CMAKE_AUTOMOC ON)

set(SOURCES
    Logger.cpp
    Logger.h
    ModelAdapter.cpp
    ModelAdapter.h
    MCPClient.cpp
    MCPClient.h
    Host.cpp
    Host.h
    main.cpp
)

add_executable(${TARGET} ${SOURCES})

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(spdlog CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)

target_link_libraries(${TARGET}
    PUBLIC
    Qt6::Core
    PRIVATE
    httplib::httplib
    spdlog::spdlog_header_only
)

# 修改从VS调试时的工作目录路径
set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_OUTPUT_DIRECTORY_INTDIR})

# 为config.json创建符号链接
add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/config.json ${CMAKE_OUTPUT_DIRECTORY_INTDIR}/config.json
)

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
find_package(OpenSSL REQUIRED)
find_package(httplib CONFIG REQUIRED)

target_link_libraries(${TARGET}
    PUBLIC
    Qt6::Core
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    httplib::httplib
    spdlog::spdlog_header_only
)

add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

# 修改从VS调试时的工作目录路径
set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_OUTPUT_DIRECTORY_INTDIR})

# 为config.json创建符号链接
add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/config.json ${CMAKE_OUTPUT_DIRECTORY_INTDIR}/config.json
)

install(TARGETS ${TARGET} RUNTIME DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION .)
# 安装所依赖的dll（需要在vcvarsall.bat打开的环境下使用）
set(APP "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${TARGET}.exe")
set(DIRS "${CMAKE_OUTPUT_INSTALL_INTDIR}")
install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APP}\"  \"\"  \"${DIRS}\")
")
set(TARGET MCPServer)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES
    MCPServer.h
    MCPServer.cpp
)

add_library(${TARGET} SHARED ${SOURCES})

target_compile_definitions(${TARGET} PRIVATE BUILD_MCPSERVER)

target_link_libraries(${TARGET}
    PUBLIC
    Qt6::Core
)

target_include_directories(${TARGET}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/MCPServer
)

install(TARGETS ${TARGET} RUNTIME DESTINATION .)

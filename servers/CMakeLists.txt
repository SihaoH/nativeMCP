set(CMAKE_AUTOMOC ON)

# server_time
add_executable(server_time server_time.h server_time.cpp)
target_link_libraries(server_time PRIVATE MCPServer)
set_target_properties(server_time PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_OUTPUT_DIRECTORY_INTDIR}
    FOLDER "servers"
)
install(TARGETS server_time RUNTIME DESTINATION .)

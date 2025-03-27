set(CMAKE_AUTOMOC ON)

set(SERVER_LIST )

# server_test
add_executable(server_test server_test.h server_test.cpp)
list(APPEND SERVER_LIST server_test)

# server_time
add_executable(server_time server_time.h server_time.cpp)
list(APPEND SERVER_LIST server_time)


foreach(SERVER_TARGET ${SERVER_LIST})
    target_link_libraries(${SERVER_TARGET} PRIVATE MCPServer)
    set_target_properties(${SERVER_TARGET} PROPERTIES  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_OUTPUT_DIRECTORY_INTDIR} FOLDER "servers")
    install(TARGETS ${SERVER_TARGET} RUNTIME DESTINATION .)

    # 安装所依赖的dll（需要在vcvarsall.bat打开的环境下使用）
    set(APP "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${SERVER_TARGET}.exe")
    set(DIRS "${CMAKE_OUTPUT_INSTALL_INTDIR}")
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APP}\"  \"\"  \"${DIRS}\")
    ")
endforeach()

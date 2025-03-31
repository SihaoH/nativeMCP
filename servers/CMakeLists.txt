set(CMAKE_AUTOMOC ON)

set(SERVER_LIST )

# screen_parser
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(OpenSSL REQUIRED)
find_package(httplib CONFIG REQUIRED)
add_executable(screen_parser screen_parser.h screen_parser.cpp)
target_link_libraries(screen_parser PRIVATE
    Qt6::Gui
    OpenSSL::SSL
    OpenSSL::Crypto
    httplib::httplib
)
list(APPEND SERVER_LIST screen_parser)

# computer_use
add_executable(computer_use computer_use.h computer_use.cpp)
list(APPEND SERVER_LIST computer_use)


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

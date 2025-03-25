cmake_minimum_required(VERSION 3.30)

# 配置vcpkg的相关内容
set(VCPKG_TARGET_TRIPLET "x64-windows")
set(VCPKG_LIBRARY_LINKAGE dynamic)

find_program(VCPKG_ROOT vcpkg.exe PATHS $ENV{PATH})
cmake_path(GET VCPKG_ROOT PARENT_PATH VCPKG_ROOT)
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(PROJ_NAME nativeMCP)
project(${PROJ_NAME} VERSION 0.1.0)

# 启用解决方案文件夹
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_OUTPUT_DIRECTORY_INTDIR ${PROJECT_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR})
set(CMAKE_OUTPUT_INSTALL_INTDIR ${PROJECT_BINARY_DIR}/bin/$<CONFIG>)

add_subdirectory(MCPServer)
add_subdirectory(servers)
add_subdirectory(host)

# 安装所依赖的dll（需要在vcvarsall.bat打开的环境下使用）
set(APP "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/server.exe")
set(DIRS "${CMAKE_OUTPUT_INSTALL_INTDIR}")
install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APP}\"  \"\"  \"${DIRS}\")
   ")


set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
include(InstallRequiredSystemLibraries)
if (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
endif (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)

# CPack 基本设置
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "nativeRAG Application")
set(CPACK_PACKAGE_VENDOR "SihaoH")

# 指定打包 Release 版本
set(CPACK_BUILD_CONFIG "Release")
set(CPACK_CONFIGURATION_TYPES "Release")
set(CPACK_GENERATOR "ZIP")

# 必须在设置完所有 CPACK 变量后包含 CPack
include(CPack)

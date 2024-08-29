cmake_minimum_required(VERSION 3.25)

set(VCPKG_MANIFEST_MODE ON)
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

if(CMAKE_HOST_LINUX)
    set(VCPKG_TARGET_TRIPLET "x64-linux-dynamic")
endif()

project("ImFiddle")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 23)

add_subdirectory("ImFiddle")
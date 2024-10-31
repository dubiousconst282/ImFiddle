cmake_minimum_required(VERSION 3.25)

set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/build")

project("ImFiddle")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 23)

if("${CPM_SOURCE_CACHE}" MATCHES "OFF")
    set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/CPM_cache/")
endif()

include("deps/CPM.cmake")

add_subdirectory("ImFiddle")
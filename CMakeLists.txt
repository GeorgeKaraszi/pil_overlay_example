cmake_minimum_required(VERSION 3.16)
project(pil_overlay_example)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DENVIRONMENT64)
else()
    add_definitions(-DENVIRONMENT32)
endif()

add_subdirectory(MinHook)
add_subdirectory(directx_test_window)
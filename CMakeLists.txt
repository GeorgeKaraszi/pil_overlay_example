cmake_minimum_required(VERSION 3.16)
project(pil_overlay_example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(VCPKG_TARGET_ARCHITECTURE x64)
    add_definitions(-DENVIRONMENT64)
else()
    set(VCPKG_TARGET_ARCHITECTURE x86)
    add_definitions(-DENVIRONMENT32)
endif()


if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Building RELEASE!")
    add_compile_options(/O2)
else()
    add_definitions(-DOVERLAY_DEBUG)
endif()



add_subdirectory(MinHook)
add_subdirectory(directx_test_window)
add_subdirectory(ps_websockets_example)
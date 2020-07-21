cmake_minimum_required(VERSION 3.16)
project(directx_test_window)

add_subdirectory(../Overlay ./overlay_build)
file(GLOB CORE_FILES "*.h" "*.hpp" "*.cpp" "*.rc")

add_executable(directx_test_window WIN32 ${CORE_FILES})
target_link_libraries(directx_test_window d3d11 d3dcompiler)
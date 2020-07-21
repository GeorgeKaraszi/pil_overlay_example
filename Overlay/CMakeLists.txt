cmake_minimum_required(VERSION 3.16)
project(overlay)

include(CMakePackageConfigHelpers)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
set(OPENSSL_USE_STATIC_LIBS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

add_definitions(-D_WIN32_WINNT=0x0601)

find_library(DirectX_D3D11_LIBRARY d3d11)
find_package(Boost COMPONENTS REQUIRED date_time system coroutine regex)
find_package(OpenSSL)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

file(GLOB IMGUI_FILES       "imgui/*.h" "imgui/*.hpp" "imgui/*.cpp")
file(GLOB CORE_FILES        "Core/*.h" "Core/*.hpp" "Core/*.cpp")
file(GLOB PS_FILES          "Planetside/*.h" "Planetside/*.hpp" "Planetside/*.cpp")
file(GLOB WEBSOCKET_FILES   "WebSockets/*.h" "WebSockets/*.hpp" "WebSockets/*.cpp")
file(GLOB MENU_LAYER_FILES  "MenuLayer/*.h" "MenuLayer/*.hpp" "MenuLayer/*.cpp")
file(GLOB MAIN_FILES         "*.h" "*.hpp" "*.cpp")

add_library(overlay SHARED
        ${MAIN_FILES}
        ${CORE_FILES}
        ${PS_FILES}
        ${MENU_LAYER_FILES}
        ${IMGUI_FILES}
        ${WEBSOCKET_FILES}
        )
target_link_libraries(overlay minhook ${DirectX_D3D11_LIBRARY} ${Boost_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_LIBRARIES})


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Roboto-Medium.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
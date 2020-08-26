#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_PCH_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_PCH_HPP

#include "NetworkLayer/boost.hpp"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Core/layer.hpp"
#include "Core/layer_stack.hpp"

#include "MenuLayer/menu_layer.hpp"
#include "NetworkLayer/network_layer.hpp"
#include "MenuLayer/log_layer.hpp"

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_PCH_HPP

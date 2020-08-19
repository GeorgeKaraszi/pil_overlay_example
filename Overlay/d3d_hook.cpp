#include "pch.hpp"

static std::once_flag g_menu_is_attached;
static WNDPROC lpOriginalWindowProc  = nullptr;
static Overlay::D3DHook* s_Instance  = nullptr;

//===================================================================================================================
// D3D Callback Hook Methods

HRESULT WINAPI PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
  auto hook = Overlay::D3DHook::GetHook();
  hook->RenderStack(pSwapChain);

  return hook->D3D11PresentPtr(pSwapChain, SyncInterval, Flags);
}

//-----------------------------------------------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

  if(Overlay::D3DHook::GetHook()->CanDisplayMenu())
  {
    if(uMsg != WM_CLOSE && uMsg != WM_DESTROY && uMsg != WM_SIZING)
    {
      ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
  }

  return CallWindowProc(lpOriginalWindowProc, hwnd, uMsg, wParam, lParam);
}

//===================================================================================================================
// Overlay::D3DHook Class Methods

namespace Overlay
{
  D3DHook::D3DHook(HMODULE hModule)
  : m_hModule(hModule),
    m_hWnd(GetForegroundWindow()),
    m_dll_path(LocateDllPath(hModule)),
    m_network(m_dll_path),
    m_menuLayer()
  {
    assert(BLANK_PTR(s_Instance) && "There exists an Instance of D3DHook already!");
    s_Instance           = this;
    lpOriginalWindowProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
  }

  //-----------------------------------------------------------------------------------------------------------------
  bool D3DHook::Start()
  {
    if(!SetupSwapChain())
    {
      MessageBoxW(m_hWnd, L"Failed to create device and swapchain.", L"Fatal Error", MB_ICONERROR);
      return false;
    }

    SetupVTables();

    if(!SetupHooks())
    {
      MessageBoxW(m_hWnd, L"Failed to establish DirectX.", L"Fatal Error", MB_ICONERROR);
      return false;
    }

    return true;
  }

  //-----------------------------------------------------------------------------------------------------------------
  void D3DHook::Run()
  {
    auto sleep_time = std::chrono::milliseconds(500);

    while(!(GetAsyncKeyState(VK_DELETE) & 0x1))
    {
      m_network.async_run();
      std::this_thread::sleep_for(sleep_time);
    }
  }

  //-----------------------------------------------------------------------------------------------------------------
  void D3DHook::Shutdown()
  {
    if(PRESENT_PTR(d3dDevicePtr))  { d3dDevicePtr->Release();  }
    if(PRESENT_PTR(d3dContextPtr)) { d3dContextPtr->Release(); }
    if(PRESENT_PTR(swapChainPtr))  { swapChainPtr->Release();  }
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)lpOriginalWindowProc);

    m_network.Shutdown();
    m_menuLayer.OnDetach();
    FreeLibraryAndExitThread(m_hModule, 0);
  }

  //-----------------------------------------------------------------------------------------------------------------

  void D3DHook::RenderStack(IDXGISwapChain *swap_chain)
  {
    AttachMenu(swap_chain);

    if (GetAsyncKeyState(VK_F9) & 0x1)
    {
      m_menuLayer.ToggleRunnable();
    }

    m_menuLayer.OnEvent();
  }

  //-----------------------------------------------------------------------------------------------------------------
  bool D3DHook::SetupSwapChain()
  {
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
    D3D_FEATURE_LEVEL obtainedLevel;
    DXGI_SWAP_CHAIN_DESC sd;
    {
      ZeroMemory(&sd, sizeof(sd));
      sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
      sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
      sd.Windowed                           = ((ULONG)GetWindowLongPtr(m_hWnd, GWL_STYLE) & WS_POPUP) == 0;
      sd.BufferCount                        = 1;
      sd.SampleDesc.Count                   = 1;
      sd.BufferDesc.Width                   = 1;
      sd.BufferDesc.Height                  = 1;
      sd.BufferDesc.RefreshRate.Numerator   = 0;
      sd.BufferDesc.RefreshRate.Denominator = 1;
      sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
      sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
      sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
      sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      sd.OutputWindow                       = m_hWnd;
    }

    auto h_result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        levels,
        sizeof(levels) / sizeof(D3D_FEATURE_LEVEL),
        D3D11_SDK_VERSION,
        &sd,
        &swapChainPtr,
        &d3dDevicePtr,
        &obtainedLevel,
        &d3dContextPtr);


    return SUCCEEDED(h_result);
  }

  //-----------------------------------------------------------------------------------------------------------------
  void D3DHook::SetupVTables()
  {
    SwapChainVTablePtr     = (DWORD_PTR*)(swapChainPtr);
    SwapChainVTablePtr     = (DWORD_PTR*)(SwapChainVTablePtr[0]);

    DeviceVTablePtr        = (DWORD_PTR*)(d3dDevicePtr);
    DeviceVTablePtr        = (DWORD_PTR*)DeviceVTablePtr[0];

    DeviceContextVTablePtr = (DWORD_PTR*)(d3dContextPtr);
    DeviceContextVTablePtr = (DWORD_PTR*)(DeviceContextVTablePtr[0]);
  }

  //-----------------------------------------------------------------------------------------------------------------
  void D3DHook::AttachMenu(IDXGISwapChain* swap_chain)
  {
    std::call_once(g_menu_is_attached, [&]{
      swap_chain->GetDevice(__uuidof(d3dDevicePtr), reinterpret_cast<void**>(&d3dDevicePtr));
      d3dDevicePtr->GetImmediateContext(&d3dContextPtr);
      m_menuLayer.OnAttach();
    });
  }

  //-----------------------------------------------------------------------------------------------------------------
  bool D3DHook::SetupHooks()
  {
    if (MH_Initialize() != MH_OK)
    {
      return false;
    }

    if(EnableHook(SwapChainVTablePtr[8], PresentHook, D3D11PresentPtr))
    {
      DWORD old_protect;
      VirtualProtect((LPVOID)D3D11PresentPtr, 2, PAGE_EXECUTE_READWRITE, &old_protect);
      return true;
    }

    return false;
  }

  //-----------------------------------------------------------------------------------------------------------------
  template<typename vTableT, typename CallbackT, typename OrgFnT>
  bool D3DHook::EnableHook(vTableT vTableEntry, CallbackT CallbackFn, OrgFnT &OrgFN)
  {
    if(MH_CreateHook((DWORD_PTR*)vTableEntry, (LPVOID)CallbackFn, reinterpret_cast<void **>(&OrgFN)) != MH_OK)
    {
      return false;
    }

    return MH_EnableHook((DWORD_PTR*)vTableEntry) == MH_OK;
  }

  //-----------------------------------------------------------------------------------------------------------------
  std::string D3DHook::LocateDllPath(HMODULE hModule) const
  {
    char dll_path[MAX_PATH]      = "\0";
    size_t path_length           = GetModuleFileName(hModule, dll_path, MAX_PATH);
    std::string_view path_finder = { dll_path, path_length };
    return std::string(path_finder.substr(0, path_finder.find_last_of("/\\")+1));
  }

  //-----------------------------------------------------------------------------------------------------------------
  D3DHook *D3DHook::GetHook()
  {
    return s_Instance;
  }
}
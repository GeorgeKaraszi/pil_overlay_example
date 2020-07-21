#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_HOOK_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_HOOK_HPP

#include <mutex>
#include <MinHook.h>
#include "d3d_pch.hpp"
#include "MenuLayer/menu_layer.hpp"

typedef HRESULT(WINAPI *D3D11Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);


namespace Overlay
{
  class D3DHook
  {
  public:
    D3DHook()               = delete;
    D3DHook(const D3DHook&) = delete;
    explicit D3DHook(HMODULE hModule);

    //-----------------------------------------------------------------------------------------------------------------
    // Begin hooking Direct X and start rending items
    void Start();

    //-----------------------------------------------------------------------------------------------------------------
    // Release captured pointers, undo hooks, and eject the library from the current process
    void Shutdown() const;


    void RenderStack(IDXGISwapChain* swap_chain);

    HWND GetHwnd()                            { return m_hWnd; }

    [[nodiscard]]
    ID3D11Device* GetDevice()           const { return d3dDevicePtr; }

    [[nodiscard]]
    ID3D11DeviceContext* GetDeviceCtx() const { return d3dContextPtr; }

    [[nodiscard]]
    IDXGISwapChain* GetSwapChain()      const { return swapChainPtr; }

    bool   CanDisplayMenu()                   { return m_menuLayer.isRunnable(); }
    MenuLayer &GetMenu()                      { return m_menuLayer; }

    static D3DHook *GetHook();
  private:
    bool SetupSwapChain();
    void SetupVTables();
    bool SetupHooks();

    void AttachMenu(IDXGISwapChain* swap_chain);

    template<typename vTableT, typename CallbackT, typename OrgFnT>
    bool EnableHook(vTableT vTableEntry, CallbackT CallbackFn, OrgFnT &OrgFN);
  public:
    HMODULE m_hModule;
    //-----------------------------------------------------------------------------------------------------------------
    // Direct X Device & Context
    ID3D11Device*        d3dDevicePtr  { nullptr };
    ID3D11DeviceContext* d3dContextPtr { nullptr };
    IDXGISwapChain*      swapChainPtr  { nullptr };

    //-----------------------------------------------------------------------------------------------------------------
    // Direct X Hook Ptr's
    D3D11Present D3D11PresentPtr       { nullptr };

    //-----------------------------------------------------------------------------------------------------------------
    //Direct X VTable Ptr's
    DWORD_PTR*  SwapChainVTablePtr     { nullptr };
    DWORD_PTR*	DeviceVTablePtr        { nullptr };
    DWORD_PTR*  DeviceContextVTablePtr { nullptr };
  private:
    HWND      m_hWnd;
    MenuLayer m_menuLayer;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_D3D_HOOK_HPP

#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_LAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_LAYER_HPP

#include <Windows.h>
#include <string>

namespace Overlay
{
  class Layer
  {
  public:
    explicit Layer(const char *name, bool runnable = true) : m_DebugName(name), m_runnable(runnable) {};
    Layer(const Layer&) = default;
    virtual ~Layer()    = default;

    [[maybe_unused]]
    virtual void OnAttach()              {};
    [[maybe_unused]]
    virtual void OnDetach() const        {};
    [[maybe_unused]]
    virtual void OnUpdate()              {};
    [[maybe_unused]]
    virtual void OnEvent()               {};

    virtual bool isRunnable()            { return m_runnable; }
    virtual bool ToggleRunnable()        { return m_runnable = !m_runnable; }
    virtual bool SetRunnable(bool state) { return m_runnable = state; }

  protected:
    std::string m_DebugName;
    bool        m_runnable;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_LAYER_HPP

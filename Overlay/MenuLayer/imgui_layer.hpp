#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP

#include "../Core/layer.hpp"

namespace Overlay
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ImGuiLayer(const char *name, bool runnable) : Layer(name, runnable) {}
    ImGuiLayer(const ImGuiLayer&) = default;
    ~ImGuiLayer() override        = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent()  override;

    virtual void Begin();
    virtual void End();

  protected:
    virtual void ApplyGuiProfile();

  private:
  };
}
#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP

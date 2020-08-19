#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP

#include <mutex>
#include "../Core/layer.hpp"

namespace Overlay
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ImGuiLayer(const ImGuiLayer&) = default;
    ~ImGuiLayer() override        = default;

    virtual void OnAttach() override;
    void OnDetach() const override;
    void OnEvent()  override;

    virtual void Begin();
    virtual void End();

  protected:
    virtual void ApplyGuiProfile();

  private:
  };
}
#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_IMGUI_LAYER_HPP

#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_CORE_LAYER_STACK_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_CORE_LAYER_STACK_HPP

#include <vector>
#include "layer.hpp"

namespace Overlay
{
  class LayerStack
  {
  public:
    typedef Layer* pLayer;
    typedef typename std::vector<pLayer> VLayer;

    LayerStack() = default;
    ~LayerStack();

    void AttachLayers();
    void DeleteLayers();

    void PushLayer(pLayer layer);
    [[maybe_unused]]
    void PopLayer(pLayer layer);

    size_t size() const noexcept         { return m_Layers.size(); }
    Layer *operator[](size_t idx)        { return m_Layers[idx]; }
    VLayer::iterator begin()             { return m_Layers.begin(); }
    VLayer::iterator end()               { return m_Layers.end(); }

    [[nodiscard]]
    VLayer::const_iterator begin() const { return m_Layers.cbegin(); }
    [[nodiscard]]
    VLayer::const_iterator end()   const { return m_Layers.cend(); }
  private:
    VLayer m_Layers;
    unsigned int m_LayerInsertIndex = 0;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_CORE_LAYER_STACK_HPP

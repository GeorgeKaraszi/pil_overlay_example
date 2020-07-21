#include "layer_stack.hpp"

namespace Overlay
{

  LayerStack::~LayerStack()
  {
    for(pLayer layer : m_Layers)
    {
      layer->OnDetach();
    }

    m_Layers.clear();
  }

  void LayerStack::PushLayer(pLayer layer)
  {
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
  }

  [[maybe_unused]]
  void LayerStack::PopLayer(pLayer layer)
  {
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
      layer->OnDetach();
      m_Layers.erase(it);
      m_LayerInsertIndex--;
    }
  }
}
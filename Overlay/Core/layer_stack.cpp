#include "layer_stack.hpp"

namespace Overlay
{

  LayerStack::~LayerStack()
  {
    DeleteLayers();
  }

  void LayerStack::DeleteLayers()
  {
    for(size_t i = 0; i < m_Layers.size(); i++)
    {
      m_Layers[i]->OnDetach();
      delete m_Layers[i];
      m_Layers[i] = nullptr;
    }

    m_Layers.clear();
  }

  void LayerStack::AttachLayers()
  {
    for(pLayer layer : m_Layers)
    {
      layer->OnAttach();
    }
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
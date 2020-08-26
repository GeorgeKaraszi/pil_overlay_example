#ifndef PIL_OVERLAY_EXAMPLE_OVERLAY_LOG_LAYER_HPP
#define PIL_OVERLAY_EXAMPLE_OVERLAY_LOG_LAYER_HPP
#include <vector>
#include <time.h>
#include "../imgui/imgui.h"
#include "imgui_layer.hpp"
namespace Overlay
{
  class LogLayer : public ImGuiLayer
  {
  public:
    enum _LOG_LEVEL { INFO, WARNING, CRITICAL };
    typedef int LOG_LEVELT;

    struct MESSAGE
    {
      LOG_LEVELT  level { INFO };
      std::string message;

      ImVec4 color()
      {
        switch(level)
        {
          case CRITICAL: return ImColor(IM_COL32(252, 0, 0, 255));
          case WARNING:  return ImColor(IM_COL32(255, 234, 0, 255));
          default:       return ImColor(IM_COL32_WHITE);
        }
      }
    };

  public:
    LogLayer();
    LogLayer(const LogLayer&) = default;
    ~LogLayer() override          = default;

    static LogLayer* GetLogger();

    void OnAttach() override {};
    void OnDetach() override;
    void OnEvent() override;

    void clear()                                        { m_logs.clear(); }
    void addLog(const MESSAGE &msg)                     { m_logs.push_back(msg); }
    void addLog(const std::string &msg)                 { m_logs.push_back(format_message(INFO, msg)); }
    void addLog(LOG_LEVELT lvl, const std::string &msg) { m_logs.push_back(format_message(lvl, msg)); }

  private:
    MESSAGE format_message(LOG_LEVELT lvl, const std::string &msg);

  private:
    std::vector<MESSAGE> m_logs;
    ImGuiTextFilter      m_filter;
    bool                 m_autoscroll;  // Keep scrolling if already at the bottom.
  };

  typedef LogLayer::_LOG_LEVEL       LOG_LEVEL;
  typedef typename LogLayer::MESSAGE LOG_MESSAGE;
}

#endif //PIL_OVERLAY_EXAMPLE_OVERLAY_LOG_LAYER_HPP

#include "log_layer.hpp"
#include "../pch.hpp"

static Overlay::LogLayer * g_logger = nullptr;

namespace Overlay
{
  LogLayer::LogLayer() : ImGuiLayer("LogMenu", false)
  {
    assert(BLANK_PTR(g_logger) && "There exists an Instance of LogLayer already!");
    m_autoscroll = true;
    g_logger     = this;
  }

  LogLayer *LogLayer::GetLogger()
  {
    return g_logger;
  }

  void LogLayer::OnDetach()
  {
    g_logger = nullptr;
  }

  void LogLayer::OnEvent()
  {
    if(!isRunnable()) return;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Internal Logs", &m_runnable, ImGuiCond_FirstUseEver))
    {
      ImGui::End();
      return;
    }

    ImGui::Checkbox("Auto-scroll", &m_autoscroll);
    ImGui::SameLine();
    if(ImGui::Button("Clear")) { clear(); }
    ImGui::SameLine();
    if(ImGui::Button("Copy")) { ImGui::LogToClipboard(); }
    ImGui::SameLine();
    m_filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    if (m_filter.IsActive())
    {
      for (int line_no = 0; line_no < m_logs.size(); line_no++)
      {
        auto *current_line     = &m_logs[line_no];
        const char* line_start = current_line->message.begin()._Ptr;
        const char* line_end   = current_line->message.end()._Ptr;
        if (m_filter.PassFilter(line_start, line_end))
        {
          ImGui::TextColored(current_line->color(), "%s", current_line->message.c_str());
        }
      }
    }
    else
    {
      ImGuiListClipper clipper(m_logs.size());
      while (clipper.Step())
      {
        for(size_t line_num = clipper.DisplayStart; line_num < clipper.DisplayEnd; line_num++)
        {
          ImGui::TextColored(m_logs[line_num].color(), "%s", m_logs[line_num].message.c_str());
        }
      }
      clipper.End();
    }
    ImGui::PopStyleVar();

    if (m_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
      ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild(); // "scrolling"
    ImGui::End();      // "Internal Logs"
  }

  LogLayer::MESSAGE LogLayer::format_message(LogLayer::LOG_LEVELT lvl, const std::string &msg)
  {
    struct tm tm_s;
    char buff[400];
    char time_buff[80];
    time_t timestamp = time(0);
    localtime_s(&tm_s, &timestamp);
    strftime(time_buff, 80, "%r", &tm_s);
    snprintf(buff, 400, "[%s]:%s", time_buff, msg.c_str());

    return { lvl, buff };
  }
}

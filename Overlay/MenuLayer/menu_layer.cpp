#include "../pch.hpp"

namespace Overlay
{
  void MenuLayer::OnAttach()
  {
    ImGuiLayer::OnAttach();
    m_selected_menu = MENU_ITEM_SETTINGS;
  }

  void MenuLayer::OnEvent()
  {
    if(!isRunnable()) return;

    ImGuiLayer::Begin();
    auto &g                       = *ImGui::GetCurrentContext();
    auto button_size              = ImVec2(-FLT_MIN, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration;
    ImGui::SetNextWindowPos(ImVec2(g.IO.DisplaySize.x/4,  g.IO.DisplaySize.y/4));
    ImGui::SetNextWindowSize(ImVec2(g.IO.DisplaySize.x/2, g.IO.DisplaySize.y/2));


    if(ImGui::Begin("Main Menu", &m_runnable, window_flags))
    {
      ImGui::Columns(3, "Menu Buttons", false);
      TopMenuButton("Score", button_size, MENU_ITEM_SCORE);
      ImGui::NextColumn();
      TopMenuButton("Teams", button_size, MENU_ITEM_TEAMS);
      ImGui::NextColumn();
      TopMenuButton("Settings", button_size, MENU_ITEM_SETTINGS);
      ImGui::EndColumns();               // "Menu Buttons"

      ImGui::Dummy(ImVec2(0.0f, 15.0f)); // Spacer between Menu Buttons & Body
      RenderMenuBody();
      ImGui::End();      //"Main Menu Bar"
    }


    ImGui::ShowDemoWindow(&m_runnable);
    ImGuiLayer::End();
  }

  void MenuLayer::RenderMenuBody()
  {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (ImVec4)ImColor::HSV(0.6f, 0.6f, 0.6f));
    ImGui::BeginChild("MenuBody", ImVec2(-FLT_MIN, 0.0f), false, ImGuiWindowFlags_NoDecoration);

    switch(m_selected_menu)
    {
      case MENU_ITEM_SETTINGS:
        SettingsMenu();
        break;
      case MENU_ITEM_TEAMS:
        TeamsMenu();
        break;
      case MENU_ITEM_SCORE:
        ScoreMenu();
        break;
      default:
        break;
    }

    ImGui::EndChild(); // "MenuBody"
    ImGui::PopStyleColor(1);
  }

  void MenuLayer::SettingsMenu()
  {
    ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_CharsUppercase;
    ImVec2 window_sz  = ImGui::GetWindowSize();

    ImGui::SetCursorPosX(window_sz.x / 4.0f);
    ImGui::PushID("team1");
    ImGui::Text("Team 1");
    ImGui::SameLine();
    ImGui::InputTextEx("", "Outfit Tag", m_settings.team1_tag, 5, ImVec2(140,0), input_flags);
    ImGui::PopID();

    ImGui::SetCursorPosX(window_sz.x / 4.0f);
    ImGui::PushID("team2");
    ImGui::Text("Team 2");
    ImGui::SameLine();
    ImGui::InputTextEx("", "Outfit Tag", m_settings.team2_tag, 5, ImVec2(140,0), input_flags);
    ImGui::PopID();

    ImGui::Dummy(ImVec2(0.0f, 30.0f));

    ImGui::SetCursorPosX(window_sz.x / 4.0f);
    ImGui::PushID("matchtimer");
    ImGui::Text("Match Timer (Minutes)");
    ImGui::SameLine();
    ImGui::InputInt("", &m_settings.match_timer);
    ImGui::PopID();

    if(m_settings.match_timer < 0)
    {
      m_settings.match_timer = 0;
    }

    ImGui::Dummy(ImVec2(0.0f, 30.0f));
    ImGui::SetCursorPosX(window_sz.x / 4.0f);
    ImGui::SmallButton("Save");

  }

  void MenuLayer::TeamsMenu()
  {

  }

  void MenuLayer::ScoreMenu()
  {

  }

  void MenuLayer::TopMenuButton(const char* menu_name, ImVec2 &size, MenuItemType target)
  {
    static const ImGuiButtonFlags dummy_flags = ImGuiButtonFlags_Disabled | ImGuiButtonFlags_NoNavFocus;
    static const ImVec2 dummy_btn_size        = ImVec2(-FLT_MIN, 3.0f);

    // selected:   Redish-Pink
    // unselected: Light blue
    float hue = (m_selected_menu == target ? 19.0f : 12.0f) * 0.05f;

    ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));

    if(ImGui::Button(menu_name, size))
    {
      m_selected_menu = target;
    }

    ImGui::PopStyleColor(3);

    if(m_selected_menu == target)
    {
      hue = 4 * 0.05f; // Light Green under bar
      ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
      ImGui::ButtonEx("", dummy_btn_size, dummy_flags);
      ImGui::PopStyleColor(1);
    }
  }
}

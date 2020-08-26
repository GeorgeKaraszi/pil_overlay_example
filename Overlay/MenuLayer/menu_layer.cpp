#include "menu_layer.hpp"
#include "../pch.hpp"

static Overlay::MenuLayer *g_menulayer = nullptr;

namespace Overlay
{
  MenuLayer::MenuLayer() : ImGuiLayer("MenuLayer", true)
  {
    assert(BLANK_PTR(g_menulayer) && "There exists an Instance of MenuLayer already!");
    g_menulayer = this;
  }

  MenuLayer *Overlay::MenuLayer::GetMenuLayer()
  {
    return g_menulayer;
  }

  void MenuLayer::OnAttach()
  {
    ImGuiLayer::OnAttach();
    m_selected_menu = MENU_ITEM_SETTINGS;
  }

  void MenuLayer::OnDetach()
  {
    ImGuiLayer::OnDetach();
    g_menulayer = nullptr;
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

    if(PRESENT_PTR(LogLayer::GetLogger()))
    {
      LogLayer::GetLogger()->OnEvent();
    }

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
    ImGui::InputTextEx("", "Outfit Tag", m_settings.current_team1_tag, 5, ImVec2(140,0), input_flags);
    ImGui::PopID();

    ImGui::SetCursorPosX(window_sz.x / 4.0f);
    ImGui::PushID("team2");
    ImGui::Text("Team 2");
    ImGui::SameLine();
    ImGui::InputTextEx("", "Outfit Tag", m_settings.current_team2_tag, 5, ImVec2(140,0), input_flags);
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
    if(ImGui::SmallButton("Save"))
    {
      SaveSettings();
    }

    if(ImGui::SmallButton("Open logger"))
    {
      LogLayer::GetLogger()->SetRunnable(true);
    }
  }

  void MenuLayer::TeamsMenu()
  {
    ImGui::Columns(2);
    for(int i = 0; i < m_teams.size(); i++)
    {
      for(auto &player : m_teams[i])
      {
        ImGui::PushID(player.uuid.c_str());
        PlayerButton(player);
        ImGui::PopID(); //uuid
      }
      if((i + 1) != m_teams.size())
        ImGui::NextColumn();
    }
    ImGui::EndColumns();
  }

  void MenuLayer::ScoreMenu()
  {
    auto callback = [&](PlanetSide::Player *player)
    {
      ImGui::PushID(player->uuid.c_str());
      PlayerButton(*player);
      ImGui::PopID(); //uuid
    };

    ImGui::Columns(2);
    m_teams[0].ActivePlayers(callback);
    ImGui::NextColumn();
    m_teams[1].ActivePlayers(callback);
    ImGui::EndColumns();
  }

  void MenuLayer::SaveSettings()
  {
    CensusAPI::CALLBACK_T callback;
    auto census = NetworkLayer::GetNetworkLayer()->GetCensus();

    callback = std::bind(&PlanetSide::Team::InitializeRoster, &m_teams[0], std::placeholders::_1);
    census->QueueOutfitRoster(m_settings.current_team1_tag, callback);

    callback = std::bind(&PlanetSide::Team::InitializeRoster, &m_teams[1], std::placeholders::_1);
    census->QueueOutfitRoster(m_settings.current_team2_tag, callback);
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

  void MenuLayer::PlayerButton(PlanetSide::Player &player)
  {
    PlayerButton(player.GetDisplayName(), player.faction, &player.benched);
  }

  void MenuLayer::PlayerButton(const char *name, int faction, bool *benched)
  {
    static const ImGuiButtonFlags dummy_flags = ImGuiButtonFlags_NoNavFocus;
    static const ImVec2 dummy_btn_size        = ImVec2(-FLT_MIN, 30.0f);

    ImColor background;
    ImColor foreground(IM_COL32_WHITE);

    switch(faction)
    {
      case PlanetSide::Player::FACTION_VS:
        background = ImColor(140, 12, 197);
        break;
      case PlanetSide::Player::FACTION_NC:
        background = ImColor(49, 57, 247);
        break;
      default: // TR
        background = ImColor(242, 13, 29);
        break;
    }

    ImGui::PushStyleColor(ImGuiCol_Button,        background.Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, background.Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  background.Value);
    ImGui::PushStyleColor(ImGuiCol_Text,          foreground.Value);

    if(ImGui::ButtonEx(name, dummy_btn_size, dummy_flags))
    {
      ImGui::OpenPopup("bench_player_popup");
    }
    ImGui::PopStyleColor(4);

    if (ImGui::BeginPopup("bench_player_popup"))
    {
      ImGui::Checkbox("Bench Player", benched);
      ImGui::EndPopup();
    }
  }
}

#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP

#include "imgui_layer.hpp"
#include "../Planetside/team.hpp"

namespace Overlay
{
  class MenuLayer : public ImGuiLayer
  {
    enum MENU_ITEM_ {
      MENU_ITEM_NULL,
      MENU_ITEM_SCORE,
      MENU_ITEM_TEAMS,
      MENU_ITEM_SETTINGS
    };

    typedef int MenuItemType;

    struct Settings
    {
      char current_team1_tag[8] = "FIJI\0";
      char current_team2_tag[8] = "GOBS\0";
      char prev_team1_tag[8]    = "\0";
      char prev_team2_tag[8]    = "\0";
      int  match_timer          = 10;
    };
  public:
    MenuLayer() : ImGuiLayer() {};
    void OnAttach() override;
    void OnEvent() override;

    Settings *GetSettings() { return &m_settings; }

  private:
    MenuItemType m_selected_menu;
    Settings     m_settings;
    PlanetSide::Team m_teams[2] { PlanetSide::Team(), PlanetSide::Team() };

    
    void RenderMenuBody();
    void SettingsMenu();
    void TeamsMenu();
    void ScoreMenu();
    void SaveSettings();
    void TopMenuButton(const char* menu_name, ImVec2 &size, MenuItemType target);
    void PlayerButton(PlanetSide::Player *player);
    void PlayerButton(const char *name, int faction);
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP

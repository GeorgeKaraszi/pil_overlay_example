#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP

#include "imgui_layer.hpp"

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
      char team1_tag[8] = "\0";
      char team2_tag[8] = "\0";
      int  match_timer  = 10;
    };
  public:
    MenuLayer() : ImGuiLayer() {}
    void OnAttach() override;
    void OnEvent() override;

    Settings *GetSettings() { return &m_settings; }

  private:
    MenuItemType m_selected_menu;
    Settings     m_settings;
    
    void RenderMenuBody();
    void SettingsMenu();
    void TeamsMenu();
    void ScoreMenu();
    void TopMenuButton(const char* menu_name, ImVec2 &size, MenuItemType target);
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_MENULAYER_MENU_LAYER_HPP

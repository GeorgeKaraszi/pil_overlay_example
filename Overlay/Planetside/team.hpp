#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP

#include "../pch.hpp"
#include "player.hpp"
#include <map>

namespace PlanetSide
{
  class Team
  {
  public:
    int score                      { 0 };
    char Name[8]                   { "\0" };
    std::vector<Player>            Players;
    std::map<std::string, Player*> m_player_map;

    void AddScore(std::string player_uuid, int new_score)
    {
      score += new_score;
      m_player_map[player_uuid]->AddScore(new_score);
    }
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP

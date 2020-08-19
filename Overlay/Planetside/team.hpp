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
    Team()            = default;
    Team(const Team&) = default;
    Team(const JSON &team_roster) { InitializeRoster(team_roster);  }

    void InitializeRoster(const JSON &team_roster)
    {
      Reset();

      auto outfit = team_roster["outfit_list"][0];
      outfit["alias"].get_to(m_name);
      outfit["outfit_id"].get_to(m_outfit_id);

      for(auto &player : outfit["members"])
      {
        if(player["character"]["status"]["online_status"] == "1")
        {
          AddPlayer(player["character"]);
        }
      }
    }

    void ActivePlayers(std::function<void(Player*)> callback)
    {
      for(auto &x : m_players)
      {
        if(!x.benched)
          callback(&x);
      }
    }

    Player *AddPlayer(const JSON &j_player)
    {
      auto uuid = j_player["character_id"].get<std::string>();

      if(BLANK_PTR(m_player_map[uuid]))
      {
        m_players.push_back(j_player);
        m_player_map.emplace(uuid, &m_players.back());
      }

      return m_player_map[uuid];
    }

    void AddScore(std::string player_uuid, int new_score)
    {
      if(BLANK_PTR(m_player_map[player_uuid]))
        return;

      m_score += new_score;
      m_player_map[player_uuid]->AddScore(new_score);
    }

  private:
    int                            m_score { 0 };
    std::string                    m_name;
    std::string                    m_outfit_id;
    std::vector<Player>            m_players;
    std::map<std::string, Player*> m_player_map;

    void Reset()
    {
      m_player_map.clear();
      m_players.clear();
      m_score = 0;
    }
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP

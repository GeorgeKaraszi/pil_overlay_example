#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_TEAM_HPP

#include "../pch.hpp"
#include "player.hpp"
#include <map>

namespace PlanetSide
{
  class Team
  {
    typedef typename std::vector<Player> VPlayers;
  public:
    Team()            = default;
    Team(const Team&) = default;
    Team(const JSON &team_roster) { InitializeRoster(team_roster);  }

    bool InitializeRoster(const JSON &team_roster)
    {
      Reset();

      if(team_roster["returned"] == 0 ||
        !team_roster.contains("outfit_list") ||
        !team_roster["outfit_list"].is_array())
      {
        return false;
      }

      auto outfit = team_roster["outfit_list"].front();
      outfit["alias"].get_to(m_name);
      outfit["outfit_id"].get_to(m_outfit_id);

      for(auto &player : outfit["members"])
      {
        if(player["character"]["status"]["online_status"] == "1")
        {
          AddPlayer(player["character"]);
        }
      }

      return true;
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

    VPlayers::iterator begin()             { return m_players.begin(); }
    VPlayers::iterator end()               { return m_players.end(); }

    [[nodiscard]]
    VPlayers::const_iterator begin() const { return m_players.cbegin(); }
    [[nodiscard]]
    VPlayers::const_iterator end()   const { return m_players.cend(); }

  private:
    int                            m_score { 0 };
    std::string                    m_name;
    std::string                    m_outfit_id;
    VPlayers                       m_players;
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

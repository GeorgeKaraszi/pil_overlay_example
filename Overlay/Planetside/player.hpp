#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP

#include "../pch.hpp"

namespace PlanetSide
{
  class Player
  {
  public:
    enum _FACTION { FACTION_VS, FACTION_TR, FACTION_NC };
    enum _LOADOUT { LOADOUT_HA, LOADOUT_LA, LOADOUT_ENGI, LOADOUT_INFIL, LOADOUT_MEDIC, LOADOUT_MAX, LOADOUT_UNK };
    typedef int FactionT;
    typedef int LoadoutT;
  public:
    Player()               = default;
    Player(const Player&)  = default;
    Player(const JSON& player)
    {
      player["character_id"].get_to(uuid);
      player["name"]["first"].get_to(player_name);
      faction = Faction(player["faction_id"].get<std::string>()[0]);
      loadout = Loadout(player["profile"]["profile_type_description"].get<std::string>()[0]);
    }

    std::string uuid;
    std::string player_name;
    std::string alias_name;
    FactionT    faction;
    LoadoutT    loadout;
    int         score       { 0 };
    int         kill_count  { 0 };
    int         death_count { 0 };
    bool        benched     { false };

    void AddKill()               { kill_count++; }
    void AddDeath()              { death_count++; }
    void AddScore(int new_score) { score += new_score; }

    const char *GetDisplayName()
    {
      return alias_name.empty() ? player_name.c_str() : alias_name.c_str();
    }

  private:
    static FactionT Faction(char faction_id)
    {
      switch(faction_id)
      {
        case '1':
          return FACTION_VS;
        case '2':
          return FACTION_NC;
        default:
          return FACTION_TR;
      }
    }

    static LoadoutT Loadout(char profile_type)
    {
      switch(profile_type)
      {
        case 'H': // Heavy Assault
          return LOADOUT_HA;
        case 'L': // Light Assault
          return LOADOUT_LA;
        case 'C': // Combat Medic
          return LOADOUT_MEDIC;
        case 'E': // Engineer
          return LOADOUT_ENGI;
        case 'I': // Infiltrator
          return LOADOUT_INFIL;
        case 'M': // Max
          return LOADOUT_MAX;
        default:
          return LOADOUT_UNK;
      }
    }
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP

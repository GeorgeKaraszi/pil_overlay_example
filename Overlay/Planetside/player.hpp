#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP

#include "../pch.hpp"

namespace PlanetSide
{
  class Player
  {
  public:
    int score      { 0 };
    int kill_count { 0 };
    std::string player_name;
    std::string alias_name;
    std::string uuid;

    const char *GetDisplayName()
    {
      return alias_name.empty() ? player_name.c_str() : alias_name.c_str();
    }

    void AddScore(int new_score)
    {
      score += new_score;
    }
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_PLANETSIDE_PLAYER_HPP

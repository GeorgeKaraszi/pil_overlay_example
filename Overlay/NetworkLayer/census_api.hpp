#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
#include "boost.hpp"
#include "websocket.hpp"

namespace Overlay
{
  class CensusAPI
  {
  public:
    CensusAPI() = default;
    CensusAPI(WebSocket *ws);
    CensusAPI(const CensusAPI &) = default;
    JSON get_outfit_details(const std::string &outfit_tag);
    std::string get_image_data(const std::string_view &image_path);

  private:
    WebSocket   *m_ws   = 0;
    const char  *m_host = "census.daybreakgames.com";
    std::string m_outfit_lookup_schema;

    std::string make_request(const Uri &uri);
  };
}

#endif // DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
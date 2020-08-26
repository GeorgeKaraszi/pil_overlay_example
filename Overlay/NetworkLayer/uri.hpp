#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP

#include <string>

struct Uri
{
public:
  std::string ApiKey, Protocol, Host, Port, Request;

  //--------------------------------------------------------------------------------------------------------------------

  static Uri WebsocketUri(const std::string &api_key)
  {
    Uri results  = {
        .ApiKey        = api_key,
        .Protocol      = "wss",
        .Host          = "push.planetside2.com",
        .Port          = "443",
        .Request       = "/streaming?environment=ps2&service-id=s:" + api_key
    };

    return results;
  }

  //--------------------------------------------------------------------------------------------------------------------

  static Uri CensusUri(const std::string &api_key, const char* path, const char *request = nullptr)
  {
    std::stringstream fmt_request;
    fmt_request << "/s:" << api_key << path << request;

    Uri results  = {
        .ApiKey        = api_key,
        .Protocol      = "http",
        .Host          = "census.daybreakgames.com",
        .Port          = "80",
        .Request       = fmt_request.str()
    };

    return results;
  }
};  // uri

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP

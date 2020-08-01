#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP

#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <cstdio>
#include <cassert>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
    Uri results  = {
        .ApiKey        = api_key,
        .Protocol      = "http",
        .Host          = "census.daybreakgames.com",
        .Port          = "80",
        .Request       = path
    };

    if(request != nullptr)
    {
      if(request[0] != '?') results.Request += '?';
      results.Request += request + ("&service-id=s:" + api_key);
    }

    return results;
  }

  //---------------------------------------------------------------------------------------------------------------------

  [[maybe_unused]]
  static Uri Parse(const std::string &uri)
  {
    typedef std::string::const_iterator iterator_t;
    Uri results;

    if (uri.length() == 0)
    {
      return results;
    }

    std::string protocol, path, query;
    iterator_t queryStart, protocolStart, protocolEnd, hostEnd, pathStart;
    iterator_t uriEnd = uri.end();

    // get query start
    queryStart = std::find(uri.begin(), uriEnd, '?');

    // protocol
    protocolStart = uri.begin();
    protocolEnd   = std::find(protocolStart, uriEnd, ':');

    if (protocolEnd != uriEnd)
    {
      results.Protocol = std::string(protocolStart, protocolEnd);
      protocolEnd     += results.Protocol.length();
    }

    // host
    pathStart    = std::find(protocolEnd, uriEnd, '/');  // get pathStart
    hostEnd      = std::find(protocolEnd, (pathStart != uriEnd) ? pathStart : queryStart, ':');
    results.Host = std::string(protocolEnd, hostEnd);
    results.Port = (results.Protocol == "wss" || results.Protocol == "https") ? "443" : "80";

    // path
    if (pathStart != uriEnd)
    {
      path = std::string(pathStart, queryStart);
    }

    // query
    if (queryStart != uriEnd)
    {
      query = std::string(queryStart, uri.end());
    }

    results.Request = path + query;
    return results;
  }  // Parse
};  // uri

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_HPP

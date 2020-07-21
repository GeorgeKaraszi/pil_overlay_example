#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_PARSER_H
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_PARSER_H

#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>

struct Uri
{
public:
  std::string QueryString, Path, Protocol, Host, Port, HandShakePath;

  Uri()     = default;
  Uri(Uri&) = default;

  static Uri Parse(const std::string &uri)
  {
    Uri result;

    typedef std::string::const_iterator iterator_t;

    if (uri.length() == 0)
    {
      return result;
    }

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd   = std::find(protocolStart, uriEnd, ':');

    if (protocolEnd != uriEnd)
    {
      std::string protocol = &*(protocolEnd);
      if ((protocol.length() > 3) && (protocol.substr(0, 3) == "://"))
      {
        result.Protocol = std::string(protocolStart, protocolEnd);
        protocolEnd += 3;
      }
    }


    // host
    iterator_t hostStart = protocolEnd;
    iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart
    iterator_t hostEnd   = std::find(protocolEnd, (pathStart != uriEnd) ? pathStart : queryStart, ':');
    result.Host          = std::string(hostStart, hostEnd);
    result.Port          = result.Protocol == "wss" ? "443" : "80";

    // path
    if (pathStart != uriEnd)
    {
      result.Path = std::string(pathStart, queryStart);
    }

    // query
    if (queryStart != uriEnd)
    {
      result.QueryString = std::string(queryStart, uri.end());
    }

    result.HandShakePath = result.Path + result.QueryString;

    return result;

  }   // Parse
};  // uri

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_URI_PARSER_H

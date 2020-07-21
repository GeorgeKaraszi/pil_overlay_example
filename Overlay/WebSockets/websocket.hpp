#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <unordered_map>
#include "root_certificates.h"
#include "uri_parser.h"

#pragma comment (lib, "crypt32.lib")

namespace beast     = boost::beast;          // from <boost/beast.hpp>
namespace http      = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;      // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;           // from <boost/asio.hpp>
namespace ssl       = boost::asio::ssl;      // from <boost/asio/ssl.hpp>
using tcp           = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

namespace Overlay
{
  class WebSocket
  {
  public:
    WebSocket(net::io_context &ioc, ssl::context &ctx, Uri& uri);
    WebSocket(const WebSocket&) = delete;
//    ~WebSocket() { ShutDown(); }
    static WebSocket Connect();
//    bool Run();
//    bool ShutDown();
  private:
    bool m_keep_running { true };
    Uri  m_uri;
    websocket::stream<beast::ssl_stream<tcp::socket>> m_ws;
    beast::flat_buffer m_buffer;
    std::unordered_map<std::string, bool> m_dup_events;


//    void ParseEvent();
//    bool EventIsDuplicate();
//    void AddEvent();
//    void SubmitEvent();
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

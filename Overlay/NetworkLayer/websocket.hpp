#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

#include "boost.hpp"
#include "uri.hpp"

namespace Overlay
{
  // Forward declaration
  class NetworkLayer;

  class WebSocket : public std::enable_shared_from_this<WebSocket>
  {
  public:
    long long count = 0;

    WebSocket() = delete;
    explicit WebSocket(NetworkLayer* network);
    ~WebSocket();

    void submit_message(const std::string_view &msg);
    bool is_connected() { return m_ws->is_open(); }
    void disconnect();

  private:
    void connect();
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

  private:
    NetworkLayer*      m_network = 0;
    ssl_stream_ws*     m_ws      = 0;
    ssl::context       m_ctx   { ssl::context::tlsv12_client };
    Uri                m_uri;
    beast::flat_buffer m_buffer;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

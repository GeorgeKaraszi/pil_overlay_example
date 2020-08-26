#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

#include <set>
#include "boost.hpp"
#include "uri.hpp"

namespace Overlay
{
  // Forward declaration
  class NetworkLayer;

  class WebSocket : public std::enable_shared_from_this<WebSocket>
  {
  public:
    WebSocket() = delete;
    explicit WebSocket(NetworkLayer* network);
    ~WebSocket();

    void submit_message(const std::string_view &msg);
    bool is_connected() { return m_ws->is_open(); }
    void connect();
    void disconnect();

  private:
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    bool is_duplicate(const std::string_view &buffer);

  private:
    NetworkLayer*      m_network = 0;
    ssl_stream_ws*     m_ws      = 0;
    ssl::context       m_ctx   { ssl::context::tlsv12_client };
    Uri                m_uri;
    beast::flat_buffer m_buffer;
    std::hash<std::string_view> m_hash_fn;
    std::array<size_t, 10> m_message_log;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

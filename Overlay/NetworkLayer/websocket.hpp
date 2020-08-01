#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

#include "boost.hpp"
#include "root_certificates.h"
#include "uri.hpp"

#pragma comment (lib, "crypt32.lib")

namespace Overlay
{
  // Forward declaration
  class CensusAPI;

  class WebSocket : public std::enable_shared_from_this<WebSocket>
  {
  public:
    long long count = 0;

    WebSocket() = delete;
    explicit WebSocket(const Uri &stream_uri);
    ~WebSocket();

    void submit_message(const std::string_view &msg);
    void async_run()    { m_ioc.run(); }
    bool is_connected() { return m_ws->is_open(); }
    void disconnect();

  private:
    void connect();
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

  private:
    Uri                m_uri;
    net::io_context    m_ioc;
    ssl::context       m_ctx      { ssl::context::tlsv12_client };
    tcp::resolver      m_resolver { net::make_strand(m_ioc) };
    beast::flat_buffer m_buffer;
    ssl_stream_ws*     m_ws;

    friend class CensusAPI;
  };
}

#endif //DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_WEBSOCKET_HPP

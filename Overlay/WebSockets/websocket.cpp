#include "websocket.hpp"

namespace Overlay
{
//  auto const test_request =  R"({"service":"event","action":"subscribe","characters":["all"],"eventNames":["GainExperience_experience_id_4","GainExperience_experience_id_5"]})";

  WebSocket::WebSocket(net::io_context &ioc, ssl::context &ctx, Uri& uri) : m_ws(ioc, ctx), m_uri(uri)
  {
    tcp::resolver resolver{ ioc };
    auto const results = resolver.resolve(m_uri.Host, m_uri.Port);
    net::connect(get_lowest_layer(m_ws), results);

    m_ws.next_layer().handshake(ssl::stream_base::client);
    m_ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
          req.set(http::field::user_agent, "v1.0-PIL-Overlay");
        }));

    m_ws.handshake(m_uri.Host, m_uri.HandShakePath);
  }

  WebSocket WebSocket::Connect()
  {
    auto uri = Uri::Parse("wss://push.planetside2.com/streaming?environment=ps2&service-id=s:georgeapi");

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::tlsv12_client};

    // This holds the root certificate used for verification
    load_root_certificates(ctx);

    return WebSocket(ioc, ctx, uri);
  }
}
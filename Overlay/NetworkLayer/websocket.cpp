#include "websocket.hpp"
#include "root_certificates.h"
#include "../pch.hpp"

namespace Overlay
{
  WebSocket::WebSocket(NetworkLayer* network) : m_network(network)
  {
    assert(PRESENT_PTR(m_network) && "(WebSocket) Networklayer is a nullptr!");
    load_root_certificates(m_ctx);
    m_uri = Uri::WebsocketUri(m_network->m_api_key);
    m_ws  = new ssl_stream_ws{ m_network->GenerateIOC(), m_ctx };
  }

//---------------------------------------------------------------------------------------------------------------------

  WebSocket::~WebSocket()
  {
    disconnect();
    delete m_ws;
  }

//---------------------------------------------------------------------------------------------------------------------

  void WebSocket::submit_message(const std::string_view &msg)
  {
    auto buffer = net::buffer(msg);
    m_ws->async_write(
        buffer,
        beast::bind_front_handler(&WebSocket::on_write, shared_from_this())
    );
  }

  //---------------------------------------------------------------------------------------------------------------------

  void WebSocket::connect()
  {
    beast::error_code ec;
    auto const results = m_network->Resolve(m_uri);

    beast::get_lowest_layer(*m_ws).expires_after(std::chrono::seconds(15));
    beast::get_lowest_layer(*m_ws).connect(results, ec);

    if (ec)
    {
      return m_network->fail(ec, "WebSocket::connect");
    }

    beast::get_lowest_layer(*m_ws).expires_never();
    m_ws->next_layer().handshake(ssl::stream_base::client);
    m_ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    m_ws->set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
      req.set(http::field::user_agent, PIL_USER_AGENT);
      }
    ));

    m_ws->handshake(m_uri.Host, m_uri.Request, ec);
    if (ec)
    {
      m_network->fail(ec, "WebSocket::handshake");
    }
  }

//---------------------------------------------------------------------------------------------------------------------

  void WebSocket::disconnect()
  {
    if (!is_connected())
    {
      return;
    }

    m_ws->close(websocket::close_code::normal);
  }

//---------------------------------------------------------------------------------------------------------------------
// Private Methods

//---------------------------------------------------------------------------------------------------------------------

  void WebSocket::on_write(beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);
    if (ec)
      return m_network->fail(ec, "WebSocket::on_write");

    // Read a message into our buffer
    m_ws->async_read(
        m_buffer,
        beast::bind_front_handler(&WebSocket::on_read, shared_from_this())
    );
  }

//---------------------------------------------------------------------------------------------------------------------

  void WebSocket::on_read(beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);
    if (ec)
    {
      m_ws->async_read(
          m_buffer,
          beast::bind_front_handler(&WebSocket::on_read, shared_from_this())
      );

      return m_network->fail(ec, "WebSocket::on_read");
    }

    std::string_view buffer_view = { (char *) m_buffer.data().data(), m_buffer.size() };
    JSON             my_json     = JSON::parse(buffer_view);
    std::string      test;

    if (my_json["type"] == "serviceMessage")
    {
      test = my_json["payload"].dump(4);
    }
    else
    {
      test = my_json.dump(4);
    }

    m_buffer.clear();

    // Continue to add jobs to read messages
    m_ws->async_read(
        m_buffer,
        beast::bind_front_handler(&WebSocket::on_read, shared_from_this())
    );
  }

  //---------------------------------------------------------------------------------------------------------------------

  bool WebSocket::is_duplicate(const std::string_view &buffer)
  {
    size_t msg_hash      = m_hash_fn(buffer);
    size_t array_idx     = msg_hash % m_message_log.max_size();
    bool duplicate_found = m_message_log[array_idx] == msg_hash;

    m_message_log[array_idx] = msg_hash;

    return duplicate_found;
  }
}
#include "websocket.hpp"
#include <iostream>

void fail(beast::error_code ec, const char* what)
{
  std::cerr << what << ": " << ec.message() << "\n";
}

namespace Overlay
{
  WebSocket::WebSocket(const Uri &stream_uri) : m_uri(stream_uri)
  {
    load_root_certificates(m_ctx);
    m_ws = new ssl_stream_ws{ net::make_strand(m_ioc), m_ctx };
    connect();
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

  void WebSocket::disconnect()
  {
    if (!is_connected())
    {
      return;
    }

    m_ioc.stop();
    m_ws->close(websocket::close_code::normal);
  }

//---------------------------------------------------------------------------------------------------------------------
// Private Methods


  void WebSocket::connect()
  {
    beast::error_code ec;
    auto const        results = m_resolver.resolve(m_uri.Host, m_uri.Port);

    beast::get_lowest_layer(*m_ws).expires_after(std::chrono::seconds(30));
    beast::get_lowest_layer(*m_ws).connect(results, ec);

    if (ec)
    {
      return fail(ec, "connect");
    }

    beast::get_lowest_layer(*m_ws).expires_never();
    m_ws->next_layer().handshake(ssl::stream_base::client);
    m_ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    m_ws->set_option(websocket::stream_base::decorator(
        [](websocket::request_type &req) { req.set(http::field::user_agent, PIL_USER_AGENT); }
      )
    );

    m_ws->handshake(m_uri.Host, m_uri.Request, ec);
    if (ec)
    {
      fail(ec, "handshake");
    }
  }

//---------------------------------------------------------------------------------------------------------------------

  void WebSocket::on_write(beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);
    if (ec)
      return fail(ec, "write");

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
      return fail(ec, "-->on_read");


//  std::cout << beast::make_printable(m_buffer.data()) << std::endl;
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

//  std::string test = my_json.dump(4, '\t');
    std::cout << test << std::endl;

    m_buffer.clear();
    count++;

    // Continue to add jobs to read messages
    m_ws->async_read(
        m_buffer,
        beast::bind_front_handler(&WebSocket::on_read, shared_from_this())
    );
  }
}
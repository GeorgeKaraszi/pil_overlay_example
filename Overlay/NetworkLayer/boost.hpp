#ifndef PIL_OVERLAY_EXAMPLE_OVERLAY_BOOST_HPP
#define PIL_OVERLAY_EXAMPLE_OVERLAY_BOOST_HPP

#ifndef BOOST_BEAST_USE_STD_STRING_VIEW
    #define BOOST_BEAST_USE_STD_STRING_VIEW
#endif

#ifndef PIL_USER_AGENT
    #define PIL_USER_AGENT "PIL-OVERLAY-v1.0"
#endif

#include <utility>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <nlohmann/json.hpp>

namespace beast     = boost::beast;          // from <boost/beast.hpp>
namespace http      = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;       // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;            // from <boost/asio.hpp>
namespace ssl       = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp           = boost::asio::ip::tcp;   // from <boost/asio/ip/tcp.hpp>
using ssl_ws        = websocket::stream<beast::ssl_stream<tcp::socket>>;
using ssl_stream_ws = websocket::stream<beast::ssl_stream<beast::tcp_stream>>;
using JSON          = nlohmann::json;


#endif //PIL_OVERLAY_EXAMPLE_OVERLAY_BOOST_HPP

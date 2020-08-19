#ifndef PIL_OVERLAY_EXAMPLE_OVERLAY_NETWORK_LAYER_NETWORK_LAYER_HPP
#define PIL_OVERLAY_EXAMPLE_OVERLAY_NETWORK_LAYER_NETWORK_LAYER_HPP
#include "boost.hpp"
#include "websocket.hpp"
#include "census_api.hpp"

#pragma comment (lib, "crypt32.lib")

namespace Overlay
{
  class NetworkLayer
  {
  public:
    NetworkLayer()                    = delete;
    NetworkLayer(const NetworkLayer&) = delete;
    NetworkLayer(const char* api_key);
    NetworkLayer(const std::string &dll_path);
    ~NetworkLayer();

    auto GetWebsocket()    { return m_websocket; }
    auto GetCensus()       { return &m_census; }
    bool isRunning()       { return m_keep_running;  }
    void async_run();
    void Shutdown();

  private:
    bool            m_keep_running { true };
    const std::string m_api_key;
    net::io_context m_ioc;
    tcp::resolver   m_resolver { net::make_strand(m_ioc) };
    CensusAPI       m_census;
    std::shared_ptr<WebSocket> m_websocket;

    inline auto GenerateIOC()           { return net::make_strand(m_ioc); }
    inline auto Resolve(const Uri& uri) { return m_resolver.resolve(uri.Host, uri.Port); }
    std::string fetch_api_key(const std::string &dll_path);

    friend class WebSocket;
    friend class CensusAPI;
  };
}

#endif //PIL_OVERLAY_EXAMPLE_OVERLAY_NETWORK_LAYER_NETWORK_LAYER_HPP

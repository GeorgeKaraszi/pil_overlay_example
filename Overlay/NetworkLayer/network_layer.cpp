#include <fstream>
#include "../pch.hpp"
#include "network_layer.hpp"
#include "root_certificates.h"


static Overlay::NetworkLayer* g_network_layer = nullptr;

namespace Overlay
{
  NetworkLayer::NetworkLayer(const char *api_key) : Layer("NetworkLayer", true)
  {
    assert(BLANK_PTR(g_network_layer) && "There exists an Instance of NetworkLayer already!");

    m_api_key       = api_key;
    m_census        = CensusAPI(this);
    m_websocket     = std::make_shared<WebSocket>(this);
    g_network_layer = this;
  }

  NetworkLayer::NetworkLayer(const std::string &dll_path) : NetworkLayer(fetch_api_key(dll_path).c_str())
  {}

  //-----------------------------------------------------------------------------------------------------------------
  NetworkLayer::~NetworkLayer()
  {
    m_ioc.stop();
    m_websocket.reset();
    g_network_layer = nullptr;
  }

  //-----------------------------------------------------------------------------------------------------------------
  NetworkLayer *NetworkLayer::GetNetworkLayer()
  {
    return g_network_layer;
  }

  //-----------------------------------------------------------------------------------------------------------------

  void NetworkLayer::OnAttach()
  {
    m_websocket->connect();
  }

  //-----------------------------------------------------------------------------------------------------------------
  void NetworkLayer::Shutdown()
  {
    SetRunnable(false);

    if(!m_ioc.stopped())
    {
      m_ioc.stop();
    }
  }

  //-----------------------------------------------------------------------------------------------------------------
  void NetworkLayer::async_run()
  {
    if(!m_runnable)
      return;

    if(m_ioc.stopped())
      m_ioc.restart();

    m_census.PollQueue();
    m_ioc.poll_one();
  }

  //-----------------------------------------------------------------------------------------------------------------
  std::string NetworkLayer::fetch_api_key(const std::string &dll_path)
  {
    std::string api_key;
    auto file_path = dll_path + "api_key.txt";
    std::ifstream api_key_file(file_path, std::ifstream::in);
    if(api_key_file.is_open())
    {
      std::getline(api_key_file, api_key);
      boost::remove_erase_if(api_key, boost::is_any_of(" \n"));
      api_key_file.close();
    }
    else
    {
      assert(api_key_file.good() && "Could not find DBG API key file(api_key.txt).");
    }

    assert(!api_key.empty() && "(api_key.txt) Did not contain a valid API KEY inside the file.");
    return api_key;
  }

  //----------------------------------------------------------------------------------------------------------------

  void NetworkLayer::fail(beast::error_code ec, const char *what)
  {
    auto logger = LogLayer::GetLogger();
    char buffer[200];

    if(BLANK_PTR(logger))
      return;

    sprintf_s(buffer, 200, "%s:%s", what, ec.message().c_str());
    logger->addLog(LOG_LEVEL::CRITICAL, buffer);
  }
}
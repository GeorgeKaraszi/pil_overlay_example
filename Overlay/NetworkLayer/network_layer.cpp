#include <fstream>
#include "network_layer.hpp"
#include "root_certificates.h"

namespace Overlay
{
  NetworkLayer::NetworkLayer(const char *api_key) : m_api_key(api_key)
  {
    m_census    = CensusAPI(this);
    m_websocket = std::make_shared<WebSocket>(this);
  }

  NetworkLayer::NetworkLayer(const std::string &dll_path) : NetworkLayer(fetch_api_key(dll_path).c_str())
  {}

  //-----------------------------------------------------------------------------------------------------------------
  NetworkLayer::~NetworkLayer()
  {
    m_ioc.stop();
    m_websocket.reset();
  }

  //-----------------------------------------------------------------------------------------------------------------
  void NetworkLayer::Shutdown()
  {
    if(m_keep_running)
    {
      m_keep_running = false;
      m_ioc.stop();
    }
  }

  //-----------------------------------------------------------------------------------------------------------------
  void NetworkLayer::async_run()
  {
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
}
#ifndef DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
#define DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
#include "boost.hpp"
#include "uri.hpp"
#include <queue>
#include <set>

namespace Overlay
{
  class NetworkLayer;
  class CensusAPI
  {
  public:
    typedef typename std::function<bool(const JSON&)> CALLBACK_T;

  public:
    CensusAPI() = default;
    CensusAPI(NetworkLayer *network);
    CensusAPI(const CensusAPI &) = default;

    inline bool QueueEmpty() { return m_queue.empty(); }
    void QueueImageData(const std::string_view &image_path, CALLBACK_T&);
    void QueueOutfitRoster(std::string outfit_tag, CALLBACK_T&);
    void PollQueue();

  private:
    inline void AddToQueue(Uri &uri, CALLBACK_T &success)
    {
      m_queue.push(std::make_pair(uri, success));
    }

  private:
    NetworkLayer *m_network   = 0;
    std::string  m_outfit_lookup_schema;
    std::queue<std::pair<Uri, CALLBACK_T>> m_queue;

    friend class NetworkLayer;
  };
}

#endif // DIRECTX_OVERLAY_PROJECT_D3D_OVERLAY_WEBSOCKETS_CENSUS_API_HPP
#include "census_api.hpp"
#include "network_layer.hpp"
#include <iostream>

namespace Overlay
{
  CensusAPI::CensusAPI(NetworkLayer *network) : m_network(network)
  {
    assert(m_network != nullptr && "(CensusAPI) Networklayer is a nullptr!");
    std::string outfit_lookup_schema = R"(
      ?alias_lower=%s
      &c:join=outfit_member
      ^inject_at:members
      ^list:1
      (
          character
          ^inject_at:character
          ^show:character_id'name.first'faction_id'profile_id
          (
              profile
                ^on:profile_id
                ^inject_at:profile
                ^show:profile_type_description'image_path,
              characters_online_status
                ^inject_at:status
                ^hide:character_id
          )
      )
    )";

    // Strip out all the nice looking whitespace and new lines displayed in the string literal.
    boost::remove_erase_if(outfit_lookup_schema, boost::is_any_of(" \n"));
    m_outfit_lookup_schema = outfit_lookup_schema;
  }

  //---------------------------------------------------------------------------------------------------------------------

  // Gets raw image data for a profile image (http://census.daybreakgames.com/[image_path])
  // Ex: #get_image_data(outfit_member["character"]["profile"]["image_path"].get<std::string>());
  void CensusAPI::QueueImageData(const std::string_view &image_path, CALLBACK_T &callback)
  {
    auto uri = Uri::CensusUri(m_network->m_api_key, image_path.data());
    AddToQueue(uri, callback);
  }

  //---------------------------------------------------------------------------------------------------------------------

  void CensusAPI::QueueOutfitRoster(std::string outfit_tag, CALLBACK_T &callback)
  {
    char request[512];
    auto lc_tag = boost::to_lower_copy(outfit_tag);
    sprintf_s(request, 512, m_outfit_lookup_schema.c_str(), lc_tag.c_str());

    Uri uri  = Uri::CensusUri(m_network->m_api_key, "/get/ps2/outfit", request);
    AddToQueue(uri, callback);
  }

  //---------------------------------------------------------------------------------------------------------------------

  void CensusAPI::PollQueue()
  {
    if(m_queue.empty())
      return;

    Uri uri                                 = m_queue.front().first;
    CALLBACK_T callback                     = m_queue.front().second;
    auto stream                             = beast::tcp_stream(m_network->GenerateIOC());
    http::request<http::string_body>  req   = { http::verb::get, uri.Request, 11 };
    http::response<http::string_body> res;
    beast::flat_buffer                buffer;
    beast::error_code                 ec;

    req.set(http::field::host, uri.Host);
    req.set(http::field::user_agent, PIL_USER_AGENT);

    stream.connect(m_network->Resolve(uri));

    http::write(stream, req);
    http::read(stream, buffer, res);

    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    callback(JSON::parse(res.body()));
    m_queue.pop();
  }
}



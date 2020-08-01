#include "census_api.hpp"
#include <iostream>

namespace Overlay
{
  CensusAPI::CensusAPI(WebSocket *ws) : m_ws(ws)
  {
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

  JSON CensusAPI::get_outfit_details(const std::string &outfit_tag)
  {
    char request[512];
    auto lc_tag = boost::to_lower_copy(outfit_tag);
    sprintf_s(request, 512, m_outfit_lookup_schema.c_str(), lc_tag.c_str());

    auto response = make_request(Uri::CensusUri(m_ws->m_uri.ApiKey, "/get/ps2/outfit", request));
    return JSON::parse(response);
  }

//---------------------------------------------------------------------------------------------------------------------

// Gets raw image data for a profile image (http://census.daybreakgames.com/[image_path])
// Ex: #get_image_data(outfit_member["character"]["profile"]["image_path"].get<std::string>());
  std::string CensusAPI::get_image_data(const std::string_view &image_path)
  {
    return make_request(Uri::CensusUri(m_ws->m_uri.ApiKey, image_path.data()));
  }


//---------------------------------------------------------------------------------------------------------------------
// Private Methods

  std::string CensusAPI::make_request(const Uri &uri)
  {
    auto const                        results = m_ws->m_resolver.resolve(uri.Host, uri.Port);
    auto                              stream  = beast::tcp_stream(m_ws->m_ioc);
    http::request<http::string_body>  req     = { http::verb::get, uri.Request, 11 };
    http::response<http::string_body> res;
    beast::flat_buffer                buffer;
    beast::error_code                 ec;

    req.set(http::field::host, uri.Host);
    req.set(http::field::user_agent, PIL_USER_AGENT);

    stream.connect(results);

    http::write(stream, req);
    http::read(stream, buffer, res);

    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != beast::errc::not_connected)
      throw beast::system_error{ ec };

    return res.body();
  }
}



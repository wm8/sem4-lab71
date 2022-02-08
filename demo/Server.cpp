//Copyright 2022 wm8
#include "Server.h"
bool Server::isRunning;
json* Server::data;

void Server::NotFound(struct evhttp_request *request,
                      [[maybe_unused]] void *params) {
  evhttp_send_error(request,
                    HTTP_NOTFOUND, "Not Found");
}

void SendJSON(struct evhttp_request *req, json& response)
{
  struct evbuffer *buffer;
  buffer = evbuffer_new();
  evbuffer_add_printf(buffer, response.dump().c_str(), 8);
  evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type",
                    "text/html");
  evhttp_send_reply(req, HTTP_OK, "OK", buffer);
  evbuffer_free(buffer);
}

void Server::Suggest(struct evhttp_request *req,[[maybe_unused]] void *params) {
  json response;
  if (data == nullptr) {
    response["error"] = "data is not initialized!";
    SendJSON(req, response);
    return;
  }
  if (evhttp_request_get_command(req) != EVHTTP_REQ_POST) {
    response["error"] = "request method must be POST";
    SendJSON(req, response);
    return;
  }
  auto *postBuf = evhttp_request_get_input_buffer(req);
  try
  {
    json postJSON;
    {
      size_t len = evbuffer_get_length(postBuf);
      char *str;
      str = static_cast<char *>(malloc(len + 1));
      evbuffer_copyout(postBuf, str, len);
      str[len] = '\0';
      postJSON = json::parse(str);
    }
    if (!postJSON.contains("input")) {
      response["error"] = "wrong json";
      SendJSON(req, response);
      return;
    }
    auto word = postJSON["input"].get<std::string>();
    std::string res;
    response["suggestions"] = json::array();
    std::vector<std::pair<int, std::string>> words;
    for (auto &elm : *data)
      if (elm["id"].get<std::string>() == word)
        words.emplace_back(elm["cost"].get<int>(),
                           elm["name"].get<std::string>());
    struct
    {
      bool operator()(std::pair<int, std::string> a,
                      std::pair<int, std::string> b) const {
        return a.first < b.first;
      }
    } customLess;
    std::sort(words.begin(), words.end(), customLess);
    int i = 0;
    for (auto &_p : words) {
      json elm;
      elm["text"] = _p.second;
      elm["position"] = i;
      response["suggestions"].push_back(elm);
      i++;
    }

  } catch (json::parse_error &er) {
    response["error"] = er.what();
  }
  SendJSON(req, response);
}

Server::Server(const char *address, const int port)
{
  isRunning = true;
  struct event_base *ebase;
  struct evhttp *server;
  ebase = event_base_new ();
  server = evhttp_new (ebase);
  //evhttp_set_allowed_methods (server, EVHTTP_REQ_GET);
  evhttp_set_cb (server, "/v1/api/suggest", Suggest, 0);

  evhttp_set_gencb (server, NotFound, 0);
  if (evhttp_bind_socket (server, address, port) != 0)
    std::cout << "Failed to init http server." << std::endl;

  event_base_dispatch(ebase);
  evhttp_free (server);
  event_base_free (ebase);
}

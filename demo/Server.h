//Copyright 2022 wm8

#ifndef TEMPLATE_SERVER_H
#define TEMPLATE_SERVER_H
#include <iostream>
#include <algorithm>
#include <evhttp.h>
#include "nlohmann/json.hpp"
using nlohmann::json;
class Server {
 public:
  static bool isRunning;
  static std::string jsonPath;
  static json* data;
  Server(const char* address="127.0.0.1", const int port=5555);
  static void NotFound (struct evhttp_request *request, void *params);
  static void Suggest (struct evhttp_request *request, void *params);
  static void SaveWord(std::string word);
};

#endif  // TEMPLATE_SERVER_H

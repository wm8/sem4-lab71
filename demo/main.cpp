//Copyright 2022 wm8

#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <fstream>
#include "Server.h"
void LoadData(std::string filePath)
{
  while (Server::isRunning)
  {
    std::ifstream jsonFile(filePath);
    if (!jsonFile)
      throw std::runtime_error{"unable to open json: " + filePath};
    json data;
    jsonFile >> data;
    Server::data = new json(data);
    std::cout << "reloaded data: " << Server::data->dump() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}
void StartServer()
{
  [[maybe_unused]]
  auto* s = new Server("127.0.0.1", 5555);
}
int main ()
{
  std::thread th2(StartServer);
  Server::jsonPath = "/home/user/CLionProjects/lab-07-http-server/data/suggestions.json";
  std::thread th1(LoadData, Server::jsonPath);
  std::cout << "Server is running!" << std::endl;
  th1.join();
  th2.join();
  std::cout << "Server has finished!" << std::endl;

}
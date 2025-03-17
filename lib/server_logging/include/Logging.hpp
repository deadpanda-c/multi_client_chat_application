#pragma once

#include <iostream>
#include <string>
#include <ctime>

class Logging
{
  public:
      static void Log(const std::string& message)
      {
        time_t now = time(0);
        char* dt = ctime(&now);

        std::string dateTime(dt);
        dateTime.pop_back();

        std::cout << "\033[1;34m" << "[" << dateTime << "]: " << "\033[0m" << message << std::endl;
      }

      static void LogError(const std::string& message)
      {
        time_t now = time(0);
        char* dt = ctime(&now);

        std::string dateTime(dt);
        dateTime.pop_back();

        std::cout << "\033[1;31m" << "[" << dateTime << "]: " << "\033[0m" << message << std::endl;
      }

      static void LogWarning(const std::string& message)
      {
        time_t now = time(0);
        char* dt = ctime(&now);

        std::string dateTime(dt);
        dateTime.pop_back();

        std::cout << "\033[1;33m" << "[" << dateTime << "]: " << "\033[0m" << message << std::endl;
      }
  private:
      time_t timestamp;
};

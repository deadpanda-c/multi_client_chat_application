#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

class Utils {
  public:
    static std::vector<std::string> split(const std::string &s, char delim)
    {
      std::vector<std::string> result;
      std::string item;
      std::stringstream ss(s);

      while (std::getline(ss, item, delim))
        result.push_back(item);
      return result;
    }

    static std::string join(const std::vector<std::string> &v, const std::string &delim)
    {
      std::string result = "";

      for (size_t i = 0; i < v.size(); i++) {
        result += v[i];
        if (i < v.size() - 1)
          result += delim;
      }
      return result;
    }

    static std::string getCurrentTime()
    {
      time_t now = time(0);
      struct tm tstruct;
      char buf[80];
      tstruct = *localtime(&now);
      strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
      return buf;
    }
};

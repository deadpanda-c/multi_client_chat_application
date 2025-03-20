#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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
};

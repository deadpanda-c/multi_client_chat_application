#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

/**
 * @brief Utility class for string manipulation and time formatting.
 */
class Utils {
  public:
    /**
     * @brief Splits a string into a vector of strings based on a delimiter.
     * @param s The string to split.
     * @param delim The delimiter character.
     * @return A vector of strings.
     */
    static std::vector<std::string> split(const std::string &s, char delim)
    {
      std::vector<std::string> result;
      std::string item;
      std::stringstream ss(s);

      while (std::getline(ss, item, delim))
        result.push_back(item);
      return result;
    }

    /**
     * @brief Joins a vector of strings into a single string with a delimiter.
     * @param v The vector of strings to join.
     * @param delim The delimiter string.
     * @return A single string.
     */
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

    /**
     * @brief Converts a string to lowercase.
     * @return The lowercase string.
     */
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

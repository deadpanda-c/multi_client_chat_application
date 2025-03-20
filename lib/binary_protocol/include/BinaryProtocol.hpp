#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <bitset>

class BinaryProtocol {
  public:
    static std::string encode(const std::string &message)
    {
      std::string header = "00000000";
      std::string size = std::bitset<32>(message.size()).to_string();
      std::string payload = "";

      for (char c : message)
        payload += std::bitset<8>(c).to_string();
      return header + size + payload;
    }
    static std::string decode(const std::string &message)
    {
      std::string header = message.substr(0, 8);
      std::string size = message.substr(8, 32);
      std::string payload = "";

      for (int i = 40; i < message.size(); i += 8) {
        std::string byte = message.substr(i, 8);
        payload += static_cast<char>(std::bitset<8>(byte).to_ulong());
      }
      return payload;
    }
};

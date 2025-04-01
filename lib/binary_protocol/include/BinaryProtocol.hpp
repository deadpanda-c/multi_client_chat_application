#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#define SIMPLE_MESSAGE "00000000"
#define COMMAND_MESSAGE "00000001"
#define LIST_USERS "00000010"
#define LOGIN "00000011"

class BinaryProtocol {
  public:
    static std::string encode(const std::string &message, const std::string &header)
    {
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

    // get the header of the message to detect the type of the message
    static std::string getHeader(const std::string &message)
    {
      return message.substr(0, 8);
    }
};

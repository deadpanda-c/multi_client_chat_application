#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#define SIMPLE_MESSAGE "00000000"
#define COMMAND_MESSAGE "00000001"
#define LIST_USERS "00000010"
#define LOGIN "00000011"

/**
 * @brief BinaryProtocol class
 * This class is used to encode and decode messages in binary format.
 * The messages are encoded in the following format:
 * - Header: 8 bits
 * - Size: 32 bits
 * - Payload: variable size
 */
class BinaryProtocol {
  public:
    /**
     * @brief encode
     * This function encodes a message in binary format.
     * @param message The message to encode.
     * @param header The header of the message.
     * @return The encoded message.
     */
    static std::string encode(const std::string &message, const std::string &header)
    {
      std::string size = std::bitset<32>(message.size()).to_string();
      std::string payload = "";

      for (char c : message)
        payload += std::bitset<8>(c).to_string();

      return header + size + payload;
    }

    /**
     * @brief getSize
     * This function gets the size of the message.
     * @param message The message to get the size from.
     * @return The size of the message.
     */
    static std::string getSize(const std::string &message)
    {
      return message.substr(8, 32);
    }


    /**
     * @brief decode
     * This function decodes a message in binary format.
     * @param message The message to decode.
     * @return The decoded message.
     */
    static std::string decode(const std::string &message)
    {
        if (message.size() < 40) {
            return "";
        }

        std::string header = message.substr(0, 8);
        std::string sizeBits = message.substr(8, 32);
        int messageSize = std::bitset<32>(sizeBits).to_ulong();

        if (message.size() < 40 + messageSize * 8) {
            return "";
        }

        std::string payload = "";
        for (int i = 40; i < 40 + messageSize * 8; i += 8) {
            std::string byte = message.substr(i, 8);
            payload += static_cast<char>(std::bitset<8>(byte).to_ulong());
        }

        return payload;
    }

    /**
     * @brief getHeader
     * This function gets the header of the message.
     * @param message The message to get the header from.
     * @return The header of the message.
     */
    static std::string getHeader(const std::string &message)
    {
      return message.substr(0, 8);
    }
};

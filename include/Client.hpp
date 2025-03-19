#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <exception>

class Client
{
  public:
    class ClientException : public std::exception {
      public:
        ClientException(const std::string& msg) : _msg(msg) {}
        const char* what() const noexcept override {
          return _msg.c_str();
        }
      private:
        std::string _msg;
    };
    Client();
    ~Client();

    void init(const std::string& ip, unsigned short port);
    void connect();
    void send(const std::string& msg);
    std::string receive();
    void close();


  private:
    int _fd;
    unsigned short _port;
    std::string _ip;
};

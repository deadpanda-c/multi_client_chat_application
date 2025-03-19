#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <exception>
#include <arpa/inet.h>

#define MAX_MSG_SIZE 1024
#define SOCKET_ERROR "Failed to create socket"
#define CONNECT_FAILED "Failed to connect to server"

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
    void run();
    void send(const std::string& msg);
    std::string receive();
    void close();


  private:
    int _fd;
    unsigned short _port;
    std::string _ip;
    struct sockaddr_in _addr;
    struct sockaddr_in _serv_addr;
    bool _connected;

};

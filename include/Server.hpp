#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024
#define SOCKET_CREATION_FAILED "Failed to create socket"
#define SOCKET_BIND_FAILED "Failed to bind socket"
#define SOCKET_LISTEN_FAILED "Failed to listen on socket"
#define SOCKET_ACCEPT_FAILED "Failed to accept connection"
#define SELECT_FAILED "Failed to select"
#define SERVER_NOT_RUNNING "Server is not running"
#define INVALID_CLIENT_FD "Invalid client file descriptor"
#define SOCKET_FD_IN_CLIENTS "Socket file descriptor in client"
#define SOCKET_OPT_FAILED "Failed to set socket options"

class Server {
  public:
      class ServerException : public std::exception {
      public:
          ServerException(const std::string& message) : _message(message) {}
          const char* what() const noexcept override {
              return _message.c_str();
          }
      private:
          std::string _message;
      };

      Server();
      Server(unsigned short port);
      ~Server();

      void init();
      void run();
      void stop();

      void readFromClients();
      void writeToClients();

      void addClient(int client);
      void removeClient(int client);

      void broadcast(const std::string& message);
      void sendToClient(int client, const std::string& message);

  private:
      enum MessageType {
          MESSAGE,
          COMMAND
      };
      void _initFdSets();
      void _interpretMessage(int client, const std::string& message);


      std::vector<int> _clients;
      int _socket;
      int _maxClients;
      int _serverSocket;
      int _opt;
      unsigned short _port;
      size_t _maxFd;

      fd_set _writeFds;
      fd_set _readFds;
      fd_set _exceptFds;

      struct sockaddr_in _serverAddr;
      struct sockaddr_in _clientAddr;
      socklen_t _clientAddrLen;

      bool _running;
};

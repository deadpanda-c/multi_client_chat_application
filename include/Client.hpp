
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#include <thread>
    #pragma comment(lib, "ws2_32.lib")  // Link with Winsock library
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <thread>
#endif

#include <iostream>
#include <string>
#include <atomic>
#include <exception>
#include <QApplication>

#include "Window.hpp"

#define CONNECTION_FAILED "Connection failed"
#define SOCKET_CREATION_FAILED "Socket creation failed"

class Client {
  public:
    class ClientException : public std::exception {
      public:
        ClientException(const std::string& message) : _message(message) {}
        const char* what() const noexcept override {
            return _message.c_str();
        }

      private:
        std::string _message;
    };
    Client(const std::string& serverIp, unsigned short port);
    ~Client();

    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void run();
    void login();
    int show();

  private:
    Window *_window;
    std::string _serverIp;
    unsigned short _port;
    int _socket;
    std::atomic<bool> _running;
    std::thread _receiver;
    std::thread _graphical;
    struct sockaddr_in _serverAddr;

    #ifdef _WIN32
        WSADATA _wsa;
    #endif

};


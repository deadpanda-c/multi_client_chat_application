#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <thread>
    #pragma comme#include <winsock2.h>
    #define close closesocket
    #pragma comment(lib, "ws2_32.lib")nt(lib, "ws2_32.lib")  // Link with Winsock library
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <thread>
  #include <fcntl.h>
#endif

#include <iostream>
#include <string>
#include <atomic>
#include <exception>

#include <QObject>
#include <QListWidget>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

#include "Utils.hpp"

#define CONNECTION_FAILED "Connection failed"
#define SOCKET_CREATION_FAILED "Socket creation failed"

#define TITLE "Client"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

class Client : public QWidget {
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
    Client(const std::string& serverIp, unsigned short port, const std::string &title);
    virtual ~Client();

    std::vector<std::string> initCommands();
    void sendMessage(const std::string& message);
    std::string receiveMessage();
    void run();
    void login();
    void initGraphical();
    void show();
    void addItemToSideMenu(const std::string &item);
    void addMessageToChat(QString message);

  private:
    void _parseCommand(const std::string& message, const std::string &header);
    QWidget *_window;

    QHBoxLayout *_mainLayout;
    QVBoxLayout *_chatLayout;
    QVBoxLayout *_usersLayout;
    QLineEdit *_input;
    QTextEdit *_chatContentEdit;
    QTextEdit *_usersListEdit;
    QPushButton *_sendButton;

    std::vector<QWidget*> widgets;
    std::string _serverIp;
    unsigned short _port;
    int _socket;
    std::atomic<bool> _running;
    bool _windowInitialized;
    std::thread _receiver;
    std::thread _graphical;
    struct sockaddr_in _serverAddr;
    // message from server
    std::atomic<char *> _message;

    std::vector<std::string> _availableCommands;

    #ifdef _WIN32
        WSADATA _wsa;
    #endif

};


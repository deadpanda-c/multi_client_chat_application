#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Client.hpp"

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024
#define SOCKET_CREATION_FAILED "Failed to create socket"
#define SOCKET_BIND_FAILED "Failed to bind socket"

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
    Server(unsigned int port);
    ~Server();

    void init();
    void run();
    void stop();

    void addClient(std::shared_ptr<Client> client);
    void removeClient(std::shared_ptr<Client> client);

    void broadcast(const std::string& message);

private:
    std::vector<std::shared_ptr<Client>> _clients;
    int _socket;
    int _maxClients;
    unsigned _port;
    int _serverSocket;
    size_t _maxFd;

    fd_set _writeFds;
    fd_set _readFds;

    struct sockaddr_in _serverAddr;
    bool _running;
};

#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
// network lib
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "Client.hpp"

class Server {
public:
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

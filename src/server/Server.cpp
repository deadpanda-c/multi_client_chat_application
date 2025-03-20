#include "Server.hpp"
#include "Logging.hpp"
#include "BinaryProtocol.hpp"

Server::Server()
{
  Logging::Log("Server created with default port 8080");
  _port = 8080;
  _running = false;
}

Server::Server(unsigned short port)
{
  Logging::Log("Server created with port " + std::to_string(port));
  _port = port;
  _running = false;
}

Server::~Server()
{
  if (_running)
    this->stop();
}

void Server::init()
{
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket == -1)
    throw ServerException(SOCKET_CREATION_FAILED);

  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)) < 0)
    throw ServerException(SOCKET_OPT_FAILED);
  _serverAddr.sin_family = AF_INET;
  _serverAddr.sin_addr.s_addr = INADDR_ANY;
  _serverAddr.sin_port = htons(_port);

  if (bind(_socket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    throw ServerException(SOCKET_BIND_FAILED);

  Logging::Log("Server initialized on port " + std::to_string(_port));
  if (listen(_socket, 5) < 0)
    throw ServerException(SOCKET_LISTEN_FAILED);

  Logging::Log("Server listening for incoming connections...");
  _running = true;
}

void Server::_initFdSets()
{
  FD_ZERO(&_readFds);
  FD_ZERO(&_writeFds);
  FD_ZERO(&_exceptFds);

  FD_SET(_socket, &_readFds);
  FD_SET(_socket, &_exceptFds);

  for (auto client : _clients) {
    FD_SET(client, &_readFds);
    FD_SET(client, &_exceptFds);
  }
}

void Server::readFromClients()
{
  for (auto it = _clients.begin(); it != _clients.end(); ) {
    int client = *it;

    if (FD_ISSET(client, &_readFds)) {
      char buffer[1024] = {0};
      int valread = read(client, buffer, sizeof(buffer));

      if (valread == 0) {
          Logging::LogWarning("Client disconnected: " + std::to_string(client));
          removeClient(client); 
          it = _clients.erase(it);
      } else {
          // Logging::Log("Message from " + std::to_string(client) + ": " + std::string(BinaryProtocol::decode(buffer)));
          _interpretMessage(client, buffer);
          ++it;
      }
    } else {
        ++it;
    }
  }
}


void Server::run()
{
    if (!_running)
        throw ServerException(SERVER_NOT_RUNNING);

    while (true) {
        _initFdSets();
        _maxFd = _socket;

        for (auto client : _clients) {
            if (client > _maxFd)
                _maxFd = client;
        }

        int activity = select(_maxFd + 1, &_readFds, nullptr, nullptr, nullptr);
        if (activity < 0)
            throw ServerException(SELECT_FAILED);

        // Check for new connections
        if (FD_ISSET(_socket, &_readFds)) {
            int newClient = accept(_socket, (struct sockaddr *)&_clientAddr, (socklen_t *)&_clientAddrLen);
            if (newClient < 0)
                throw ServerException(SOCKET_ACCEPT_FAILED);

            Logging::Log("New connection, socket fd is " + std::to_string(newClient));
            addClient(newClient);
        }

        // Handle client messages
        readFromClients();
    }
}


void Server::stop()
{
  close(_socket);
}

void Server::addClient(int client)
{
  _clients.push_back(client);
  Logging::Log("Client added, total clients: " + std::to_string(_clients.size()));
}


void Server::removeClient(int client)
{
    FD_CLR(client, &_readFds);
    FD_CLR(client, &_exceptFds);

    close(client);
    Logging::Log("Client removed: " + std::to_string(client));
}

void Server::broadcast(const std::string &message)
{
  for (auto client : _clients) {
    send(client, message.c_str(), message.size(), 0);
  }
}

void Server::sendToClient(int client, const std::string &message)
{
  send(client, message.c_str(), message.size(), 0);
}

void Server::_interpretMessage(int client, const std::string &message)
{
  std::string header = BinaryProtocol::getHeader(message);
  Logging::Log("Header: " + header);

  if (header == SIMPLE_MESSAGE) {
    std::string body = BinaryProtocol::decode(message);
    Logging::Log("Simple message from " + std::to_string(client) + ": " + body);
  } else if (header == COMMAND_MESSAGE) {
    std::string body = BinaryProtocol::decode(message);
    Logging::Log("Command message from " + std::to_string(client) + ": " + body);
  } else {
    Logging::LogWarning("Unknown message from " + std::to_string(client) + ": " + message);
  }
}

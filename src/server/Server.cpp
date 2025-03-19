#include "Server.hpp"
#include "Logging.hpp"

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

  _serverAddr.sin_family = AF_INET;
  _serverAddr.sin_addr.s_addr = INADDR_ANY;
  _serverAddr.sin_port = htons(_port);

  if (bind(_socket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    throw ServerException(SOCKET_BIND_FAILED);

  Logging::Log("Server initialized on port " + std::to_string(_port));
  // listen for incoming connections
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
  char buffer[1024] = {0};
  int valread;
  int client;
  std::vector<int> disconnectedClients;

  for (auto it = _clients.begin(); it != _clients.end(); ++it) {
    client = *it;
    valread = read(client, buffer, 1024);

    if (valread == 0) {
      Logging::LogWarning("Client disconnected");
      disconnectedClients.push_back(client);
    } else if (valread > 0) {
      Logging::Log("Message received: " + std::string(buffer));
    } else {
      Logging::LogWarning("Error reading from client");
    }
  }

  for (int client : disconnectedClients) {
    removeClient(client);
  }
}

void Server::run()
{
  if (!_running)
    throw ServerException(SERVER_NOT_RUNNING);

  while (true) {
    _initFdSets();

    _maxFd = _socket;

    // _clients is a vector of int
    for (auto client : _clients) {
      FD_SET(client, &_readFds);
      FD_SET(client, &_exceptFds);

      if (client > _maxFd)
        _maxFd = client;

      if (client < 0)
        throw ServerException(INVALID_CLIENT_FD);

      if (client == _socket)
        throw ServerException(SOCKET_FD_IN_CLIENTS);
    }

    int activity = select(_maxFd + 1, &_readFds, &_writeFds, &_exceptFds, NULL);

    if (activity < 0)
      throw ServerException(SELECT_FAILED);

    if (FD_ISSET(_socket, &_readFds)) {
      int newClient = accept(_socket, (struct sockaddr *)&_clientAddr, (socklen_t *)&_clientAddrLen);

      if (newClient < 0)
        throw ServerException(SOCKET_ACCEPT_FAILED);

      Logging::Log("New connection, socket fd is " + std::to_string(newClient));
      addClient(newClient);
    } else {
      for (auto client : _clients) {
        if (FD_ISSET(client, &_readFds)) {
          std::cout << "Client " << client << " is ready to read" << std::endl;
          readFromClients();
        }
      }
    }
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

  for (auto it = _clients.begin(); it != _clients.end(); ++it) {
    if (*it == client) {
      _clients.erase(it);
      break;
    }
  }

  Logging::Log("Client removed, total clients: " + std::to_string(_clients.size()));
}

#include "Server.hpp"

Server::Server()
{
  _port = 8080;
  _running = false;
}

Server::Server(unsigned int port)
{
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
    throw std::runtime_error("Failed to create socket");

  _serverAddr.sin_family = AF_INET;
  _serverAddr.sin_addr.s_addr = INADDR_ANY;
  _serverAddr.sin_port = htons(_port);
}

void Server::run()
{
}

void Server::stop()
{
}

void Server::addClient(std::shared_ptr<Client> client)
{
  _clients.push_back(client);
}

void Server::removeClient(std::shared_ptr<Client> client)
{
  _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

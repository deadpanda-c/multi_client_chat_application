#include "Server.hpp"
#include "Logging.hpp"
#include "BinaryProtocol.hpp"
#include "Utils.hpp"
#include <vector>

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
  initCommands();
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

void Server::commandHelp(int client, const std::string &message)
{
  std::string helpMessage = "Available commands:\n";
  for (auto command : _commands) {
    helpMessage += command.first + "\n";
  }

  sendToClient(client, BinaryProtocol::encode(helpMessage, SIMPLE_MESSAGE));
}

void Server::commandList(int client, const std::string &message)
{
  (void)message; // unused

  std::string listMessage = "";
  for (auto client : _clients) {
    listMessage += _clientsNames[client] + "\n";
  }

  sendToClient(client, BinaryProtocol::encode(listMessage, LIST_USERS));
}

void Server::clientLogin(int client, const std::string &name)
{
  _clientsNames[client] = (_checkIfLoggedIn(client, name)) ? name + std::to_string(_loggedInClients.size() + 1) : name;

  Logging::Log("Client logged in: " + name);
  for (auto client : _clients) {
    commandList(client, "");
  }

  for (auto name : _clientsNames) {
    Logging::Log("Client: " + name.second);
  }
}

void Server::initCommands()
{
  _commands[LOGIN] = &Server::clientLogin;
  _commands[SIMPLE_MESSAGE] = &Server::commandsMessage;
  _commands[LIST_USERS] = &Server::commandList;
}

void Server::commandsMessage(int client, const std::string &message)
{
  std::vector<std::string> tokens = Utils::split(message, ' ');

  if (tokens.size() < 3) {
    Logging::LogWarning("Invalid message format: " + message);
    return;
  }

//   int targetClient = std::stoi(tokens[1]);
  //std::string final_message = std::to_string(client) + ": " + tokens[2];
  std::string final_message = _clientsNames[client] + ": " + Utils::join(std::vector<std::string>(tokens.begin() + 2, tokens.end()), " ");
  // sendToClient(targetClient, BinaryProtocol::encode(final_message, SIMPLE_MESSAGE));
  broadcast(final_message);
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

    _clientAddrLen = sizeof(_clientAddr);
    // Check for new connections
    if (FD_ISSET(_socket, &_readFds)) {
      int newClient = accept(_socket, (struct sockaddr *)&_clientAddr, (socklen_t *)&_clientAddrLen);
      if (newClient < 0) {
          perror("accept");
          throw ServerException(SOCKET_ACCEPT_FAILED + std::to_string(newClient));
      }

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
  // send the list of clients to all clients
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
  std::string body = BinaryProtocol::encode(message, SIMPLE_MESSAGE);

  for (auto client : _clients) {
    sendToClient(client, body);
    Logging::Log("Broadcasting message to " + std::to_string(client) + ": " + message);
  }
}

void Server::sendToClient(int client, const std::string &message)
{
  send(client, message.c_str(), message.size(), 0);
}

void Server::_interpretMessage(int client, const std::string &message)
{
  std::string header = BinaryProtocol::getHeader(message);
  int targetClient = 0;

  Logging::Log("Header: " + header);
  std::string body = BinaryProtocol::decode(message);
  std::cout << "BODY: " << body << std::endl;

  for (auto command : _commands) {
    if (header == command.first) {
      (this->*command.second)(client, body);
      return;
    }
  }
}
bool Server::_checkIfLoggedIn(int client, const std::string &message)
{
  for (auto name : _clientsNames) {
    if (name.second == message) {
      Logging::LogWarning("Client already logged in: " + message);
      return true;
    }
  }
  return false;
}

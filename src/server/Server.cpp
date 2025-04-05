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
  Logging::Log("Server destroyed");
}

void Server::init()
{
  initCommands();
  initDatabase();

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

void Server::commandList(int client, const std::string &body)
{
  (void)body; // Unused parameter

  std::string listMessage = "";

  if (_clientsNames.size() == 0) {
    Logging::LogWarning("No clients connected");
    return;
  }
  for (auto client : _clients) {
    listMessage += _clientsNames[client] + ",";
  }

  sendToClient(client, BinaryProtocol::encode(listMessage, LIST_USERS));
}

void Server::clientLogin(int client, const std::string &body)
{
  std::string name = BinaryProtocol::decode(body);

  int count = 0;
  std::string new_name = "";

  count = std::count(_loggedInClients.begin(), _loggedInClients.end(), name);

  new_name = (count > 0) ? name + std::to_string(count) : name;
  _clientsNames[client] = (_checkIfLoggedIn(client, name)) ? new_name : name;
  Logging::Log("Client " + std::to_string(client) + " logged in as " + _clientsNames[client]);

  _loggedInClients.push_back(new_name);
  Logging::Log("Client just logged in");

  sendToClient(client, BinaryProtocol::encode(_clientsNames[client], LOGIN));
  for (auto client : _clients) {
    commandList(client, "");
  }

  saveClientToDatabase(client, _clientsNames[client]);
}

void Server::initCommands()
{
  _commands[LOGIN] = &Server::clientLogin;
  _commands[SIMPLE_MESSAGE] = &Server::commandsMessage;
  _commands[LIST_USERS] = &Server::commandList;
}

void Server::initDatabase()
{
  if (std::filesystem::is_directory(DB_PATH)) {
    Logging::Log("Database folder exists");
    loadDatabase();
  } else {
    Logging::Log("Database folder does not exist, creating...");
    std::filesystem::create_directory(DB_PATH);
    Logging::Log("Database folder created");
  }
}

void Server::loadDatabase()
{
  for (const auto &entry : std::filesystem::directory_iterator(DB_PATH)) {
    std::string filename = entry.path().filename().string();
    std::string name = filename.substr(0, filename.find("."));
    _loggedInClients.push_back(name);
  }
}

void Server::saveClientToDatabase(int client, const std::string &name)
{
  if (!std::filesystem::is_directory(DB_PATH + name)) {
    std::filesystem::create_directory(DB_PATH + name);
    std::filesystem::create_directory(DB_PATH + name + "/messages");
    std::ofstream file(DB_PATH + name + "/info.txt");
    file << "Client: " << name << std::endl;
    file << "Account created on " << Utils::getCurrentTime() << std::endl;
    file << "Last login on " << Utils::getCurrentTime() << std::endl;
    file.close();
  }
}

int Server::getClientFileDescriptor(const std::string &clientName)
{
  for (auto client : _clientsNames) {
    if (client.second == clientName) {
      return client.first;
    }
  }
  Logging::LogWarning("Client not found");
  return -1;
}

void Server::sendPrivateMessage(int client, const std::string &decoded)
{
  // check for message history in database
  std::vector<std::string> tokens = Utils::split(decoded, ' ');
  std::string target = tokens[1];
  std::string message = (tokens.size() == 3) ? tokens[2] : "";
  std::string to = MESSAGES_FOLDER(_clientsNames[client]) + target + ".txt";
  int to_Target = getClientFileDescriptor(target);

  if (to_Target == -1) {
    Logging::LogError("Target client not found");
    return;
  }

  _clientsPrivateMessagesIndex[client] = getClientFileDescriptor(target);

  // search for a file with the target name
  if (!std::filesystem::is_directory(MESSAGES_FOLDER(_clientsNames[client]) + target)) {
    Logging::LogWarning("Target client not found");
  }

  Logging::Log("Sending private message to " + target);
  std::ofstream file(to, std::ios::app);

  if (message.size() > 0) {
    file << Utils::getCurrentTime() << " " << _clientsNames[client] << ": " << message << std::endl;
    file.close();
  }
  sendToClient(to_Target, BinaryProtocol::encode(_clientsNames[client] + ": " + message, SIMPLE_MESSAGE));
}

void Server::commandsMessage(int client, const std::string &body)
{
  std::string message = "";

  std::string decoded = BinaryProtocol::decode(body);
  std::vector<std::string> tokens = Utils::split(decoded, ' ');

  if (tokens.size() >= 3) {
    message = Utils::join(std::vector<std::string>(tokens.begin() + 2, tokens.end()), " ");
    broadcast(_clientsNames[client] + ": " + message);
  }
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
          broadcast(_clientsNames[client] + " has disconnected");

          removeClient(client);

          for (auto client : _loggedInClients)
            Logging::Log("Logged in clients: " + client);
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

}

void Server::removeClient(int client)
{
    FD_CLR(client, &_readFds);
    FD_CLR(client, &_exceptFds);

    close(client);

    if (_loggedInClients.size() > 0)
      _loggedInClients.erase(std::remove(_loggedInClients.begin(), _loggedInClients.end(), _clientsNames[client]), _loggedInClients.end());


    if (_clientsNames.find(client) != _clientsNames.end())
      _clientsNames.erase(client);

    if (_clients.size() > 0)
      _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());

    Logging::Log("Client removed, total clients: " + std::to_string(_clients.size()));
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

  std::cout << "Body: " << body << std::endl;
  for (auto command : _commands) {
    if (header == command.first) {
      (this->*command.second)(client, message);
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

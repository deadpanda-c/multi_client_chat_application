#include "Client.hpp"
#include "BinaryProtocol.hpp"

Client::Client(const std::string &serverIp, unsigned short port): _serverIp(serverIp), _port(port), _running(true) {
  #ifdef _WIN32
      WSAStartup(MAKEWORD(2, 2), &_wsa);
  #endif

  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket == -1) {
    throw ClientException(SOCKET_CREATION_FAILED);
    return;
  }
  _serverAddr.sin_family = AF_INET;
  _serverAddr.sin_port = htons(_port);
  inet_pton(AF_INET, _serverIp.c_str(), &_serverAddr.sin_addr);

  if (connect(_socket, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) {
    throw ClientException(CONNECTION_FAILED);
    return;
  }

  std::cout << "Connected to server!" << std::endl;
  _receiver = std::thread(&Client::receiveMessage, this);
  _graphical = std::thread(&Client::show, this);
}

Client::~Client()
{
  _running = false;

  if (_receiver.joinable())
      _receiver.join();
  #ifdef _WIN32
      closesocket(_socket);
      WSACleanup();
  #else
      close(_socket);
  #endif
  std::cout << "Connection closed!" << std::endl;
}


void Client::sendMessage(const std::string &message)
{
  std::string messageType = (message[0] == '/') ? COMMAND_MESSAGE : SIMPLE_MESSAGE;
  std::string binaryMessage = BinaryProtocol::encode(message, messageType);

  send(_socket, binaryMessage.c_str(), binaryMessage.size(), 0);
}

std::string Client::receiveMessage() {
  char buffer[1024] = {0};

  while (_running) {
    int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
      buffer[bytesReceived] = '\0';
      std::cout << "Server: " << BinaryProtocol::decode(buffer) << std::endl;
    //  std::cout << "You: ";
      std::cout.flush();
    } else if (bytesReceived == 0) {
      std::cout << "Server disconnected!" << std::endl;
      _running = false;
      break;
    }
  }
  return std::string(buffer);
}

void Client::login() {
  std::string username;
  std::string encodedMessage = "";

  std::cout << "Username: ";
  std::cin >> username;
  std::string cmd = std::string("/login ") + username;


  encodedMessage = BinaryProtocol::encode(cmd, COMMAND_MESSAGE);
  std::cout << "Encoded message: " << encodedMessage << std::endl;
  std::cout << "Decoded message: " << BinaryProtocol::decode(encodedMessage) << std::endl;
  sendMessage(BinaryProtocol::encode(encodedMessage, COMMAND_MESSAGE));
}

void Client::run() {
  std::string message;

  // login();
  while (true) {
    std::cout << "You: ";
    std::getline(std::cin, message);

    sendMessage(message);
  }
}

int Client::show()
{
  int ac = 0;
  char **av = nullptr;

  QApplication app(ac, av);
  _window = new Window("MyWindow");

  _window->show();
  return app.exec();
}

#include "Client.hpp"
#include "BinaryProtocol.hpp"

Client::Client(const std::string &serverIp, unsigned short port, const std::string &title)
    : _serverIp(serverIp), _port(port), _running(true), _message(NULL) {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 2), &_wsa);
#endif
    std::cout << "Client created!" << std::endl;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        throw ClientException(SOCKET_CREATION_FAILED);
    }
    std::cout << "Socket created!" << std::endl;

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);
    inet_pton(AF_INET, _serverIp.c_str(), &_serverAddr.sin_addr);

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(_socket, FIONBIO, &mode);
#else
    int flags = fcntl(_socket, F_GETFL, 0);
    fcntl(_socket, F_SETFL, flags | O_NONBLOCK);
#endif

    std::cout << "Connecting to server..." << std::endl;
    int result = ::connect(_socket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr));

    if (result == -1) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "Connection failed: " << err << std::endl;
            throw ClientException(CONNECTION_FAILED);
        }
#else
        if (errno != EINPROGRESS) {
            perror("connect");
            throw ClientException(CONNECTION_FAILED);
        }
#endif
    }

    // Wait for connection to complete (polling)
    int error = 0;

    socklen_t len = sizeof(error);
    while (true) {
        getsockopt(_socket, SOL_SOCKET, SO_ERROR, (void *)&error, &len);
        if (error == 0) {
            std::cout << "Connected to server!" << std::endl;
            break;
        } else if (error != EINPROGRESS && error != EWOULDBLOCK) {
            std::cerr << "Connection failed: " << strerror(error) << std::endl;
            throw ClientException(CONNECTION_FAILED);
        }
    }

    // Set timeout options
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    _receiver = std::thread(&Client::receiveMessage, this);
}

void Client::show() {
  QString message = "Hello, World!";
  // Window setup
    _window = new QWidget();
    _window->setWindowTitle("Chat Client");
    _window->setFixedSize(800, 600);

    // Layout setup
    _mainLayout = new QHBoxLayout();
    _sideMenu = new QListWidget();
    _chatLayout = new QVBoxLayout();

    _textEdit = new QTextEdit();
    _textEdit->setReadOnly(true);

    //_chat = new QListWidget();
    _input = new QLineEdit();
    _input->setPlaceholderText("Type your message here...");
    //_chatLayout->addWidget(_chat);

    _sendButton = new QPushButton("Send");

    // write the message to the chat
    _textEdit->setText(message);

    _chatLayout->addWidget(_textEdit);
    _chatLayout->addWidget(_input);
    _chatLayout->addWidget(_sendButton);

    QObject::connect(_sendButton, &QPushButton::clicked, this, [this]() {
        sendMessage(_input->text().toStdString());
        _input->clear();
    });


  // Set layout
    _mainLayout->addWidget(_sideMenu, 1);
    _mainLayout->addLayout(_chatLayout, 2);
    _window->setLayout(_mainLayout);

    // Show window
    _window->show();
}

Client::~Client() {
    _running = false;

    if (_receiver.joinable())
        _receiver.join();
    if (_graphical.joinable())
        _graphical.join();
#ifdef _WIN32
    closesocket(_socket);
    WSACleanup();
#else
    ::close(_socket);
#endif
    std::cout << "Connection closed!" << std::endl;
}

void Client::sendMessage(const std::string &message) {
    std::string messageType = (message[0] == '/') ? COMMAND_MESSAGE : SIMPLE_MESSAGE;
    std::string binaryMessage = BinaryProtocol::encode((messageType == SIMPLE_MESSAGE) ? std::string("/msg ") + std::to_string(0) + " " + message : message, messageType);

    send(_socket, binaryMessage.c_str(), binaryMessage.size(), 0);
}

std::string Client::receiveMessage() {
  char buffer[1024] = {0};
  std::string decodedMessage = "";
  std::string tmp = "";

  while (_running) {
    _message = new char[1024];
    int bytesReceived = recv(_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';

        decodedMessage = BinaryProtocol::decode(std::string(buffer));
        std::cout << "Decoded message: " << decodedMessage << std::endl;

        _message = (char *)memcpy(_message, decodedMessage.c_str(), sizeof(buffer));
        std::cout << "Server: " << _message << std::endl;
        tmp = std::string(_message);
        QMetaObject::invokeMethod(_textEdit, "append",
          Qt::QueuedConnection,
          Q_ARG(QString, QString::fromStdString(decodedMessage)));

    } else if (bytesReceived == 0) {
        std::cout << "Server disconnected!" << std::endl;
        _running = false;
        break;
    }
    delete[] _message;
    _message = NULL;
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

    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, message);
        sendMessage(message);
    }
}

void Client::addItemToSideMenu(QString item) {
  _sideMenu->addItem(item);
}

void Client::addMessageToChat(QString message) {

  _textEdit->setText(message);
}

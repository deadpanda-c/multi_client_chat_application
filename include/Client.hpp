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
#include <cstdlib>
#include <unordered_set>

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

#define CONNECTION_FAILED "Connection failed" // Connection error
#define SOCKET_CREATION_FAILED "Socket creation failed" // Socket creation error

#define TITLE "Client" // Window title

#define WINDOW_WIDTH 1000 // Window width
#define WINDOW_HEIGHT 800 // Window height

/**
 * @brief Client class for the chat application.
 * This class handles the client-side operations including socket communication,
 * GUI initialization, and message handling.
 */
class Client : public QWidget {
  public:
    /**
     * @brief Exception class for client errors.
     * This class is used to handle exceptions related to client operations.
     */
    class ClientException : public std::exception {
      public:
        /**
         * @brief Constructor for ClientException.
         * @param message The error message.
         */
        ClientException(const std::string& message) : _message(message) {}

        /**
         * @brief Get the error message.
         */
        const char* what() const noexcept override {
            return _message.c_str();
        }

      private:
        std::string _message; // Error message
    };
    /**
     * @brief Constructor for Client.
     * @param serverIp The IP address of the server.
     * @param port The port number of the server.
     * @param title The title of the window.
     */
    Client(const std::string& serverIp, unsigned short port, const std::string &title);

    /**
     * @brief Destructor for Client.
     * Cleans up resources and closes the socket.
     */
    virtual ~Client();

    /**
     * @brief Initialize the commands.
     * @return A vector of available commands.
     */
    std::vector<std::string> initCommands();

    /**
     * @brief Send a message to the server.
     * @param message The message to send.
     */
    void sendMessage(const std::string& message);

    /**
     * @brief Receive a message from the server, and process it.
     */
    void receiveMessage();

    /**
     * @brief Login to the server.
     */
    void login();

    /**
     * @brief Initialize the graphical interface.
     */
    void initGraphical();

    /**
     * @brief Initialize the GUI.
     */
    void show();

    /**
     * @brief Process incoming data from the server.
     * @param buffer The buffer containing the incoming data.
     */
    void processIncomingData(std::string &buffer);

  private slots:
    /**
     * @brief Slot for handling button clicks.
     * @param button The button that was clicked.
     */
    void onUserClick(QListWidgetItem *item);

  private:
    /**
     * @brief Display the connected users in the GUI in a side-menu
     * @param message The message to display.
     */
    void _displayConnectedUsers(const std::string& message, const std::string &header);


    /**
     * @brief Display the chat content in the GUI.
     * @param message The message to display.
     */
    void _displayMessage(const std::string &message);
    QWidget *_window; // Main window

    QHBoxLayout *_mainLayout; // Main layout
    QVBoxLayout *_chatLayout; // Chat layout
    QVBoxLayout *_usersLayout; // Users layout
    QListWidget *_usersList; // List of connected users

    QLineEdit *_input; // Input field for sending messages
    QTextEdit *_chatContentEdit; // Text area for displaying chat content
    QTextEdit *_usersListEdit; // Text area for displaying connected users
    QPushButton *_sendButton; // Send button

    std::vector<QWidget*> widgets; // Vector of widgets
    std::string _serverIp; // Server IP address
    std::string _username; // Username of the client
    unsigned short _port; // Server port number
    int _socket; // Socket file descriptor
    std::atomic<bool> _running; // Flag to indicate if the client is running (atomic for thread safety)
    bool _connected; // Flag to indicate if the client is connected
    bool _windowInitialized; // Flag to indicate if the window is initialized
    std::thread _receiver; // Thread for receiving messages
    std::thread _graphical; // Thread for handling the GUI
    struct sockaddr_in _serverAddr; // Server address structure
    // message from server
    std::atomic<char *> _message; // Message buffer
    std::atomic<char *> _currentPrivateUser; // Current private user
    int _currentPrivateUserIndex; // Index of the current private user
    std::string _header; // Header for the message
    int _messageSize; // Size of the message

    std::vector<std::string> _availableCommands; // Vector of available commands

    #ifdef _WIN32
        WSADATA _wsa;
    #endif

};


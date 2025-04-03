#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdbool.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <thread>

#define MAX_CLIENTS 10 // Maximum number of clients
#define MAX_BUFFER_SIZE 1024 // Maximum buffer size for messages
#define SOCKET_CREATION_FAILED "Failed to create socket" // Error message for socket creation failure
#define SOCKET_BIND_FAILED "Failed to bind socket" // Error message for socket binding failure
#define SOCKET_LISTEN_FAILED "Failed to listen on socket" // Error message for socket listening failure
#define SOCKET_ACCEPT_FAILED "Failed to accept connection" // Error message for socket acceptance failure
#define SELECT_FAILED "Failed to select" // Error message for select failure
#define SERVER_NOT_RUNNING "Server is not running" // Error message for server not running
#define INVALID_CLIENT_FD "Invalid client file descriptor" // Error message for invalid client file descriptor
#define SOCKET_FD_IN_CLIENTS "Socket file descriptor in client" // Error message for socket file descriptor in client
#define SOCKET_OPT_FAILED "Failed to set socket options" // Error message for setting socket options failure

#define DB_PATH "../db/" // Path to the database
#define MESSAGES_FOLDER(name) DB_PATH + name + "/messages/" // Path to the messages folder

/**
  * @brief Server class that handles client connections and communication.
  *
  * This class is responsible for managing client connections, sending and receiving messages,
  * and executing commands. It uses sockets for network communication and supports multiple clients.
  */
class Server {
  public:
      /**
       * @brief Exception class for server-related errors.
       * Inherits from std::exception to provide a standard interface for exceptions.
       */
      class ServerException : public std::exception {
      public:
          /**
           * Constructor that takes an error message.
           * @param message The error message to be associated with the exception.
           */
          ServerException(const std::string& message) : _message(message) {}

          /**
            * Returns the error message associated with the exception.
            * @return A C-style string containing the error message.
            */
          const char* what() const noexcept override {
              return _message.c_str();
          }
      private:
          std::string _message; ///< The error message associated with the exception.
      };

      /**
       * Constructor that initializes the server with a default port.
       */
      Server();

      /**
       * Constructor that initializes the server with a specified port.
       * @param port The port number on which the server will listen for incoming connections.
       */
      Server(unsigned short port);

      /**
       * Destructor that cleans up resources and closes the server socket.
       */
      ~Server();

      /**
       * Initializes the server by creating a socket, binding it to the specified port,
       * and setting it to listen for incoming connections.
       * @throws ServerException if any error occurs during initialization.
       */
      void init();

      /**
       * Initializes the commands that the server can execute.
       * @throws ServerException if any error occurs during initialization.
       */
      void initCommands();

      /**
       * Initializes the database by creating the necessary directories and files.
       * @throws ServerException if any error occurs during database initialization.
       */
      void initDatabase();

      /**
       * Loads the database by reading client information from files.
       * @throws ServerException if any error occurs during database loading.
       */
      void loadDatabase();

      /**
       * Saves the database by writing client information to files.
       * @throws ServerException if any error occurs during database saving.
       * @param client The client file descriptor.
       * @param name The name of the client.
       */
      void saveClientToDatabase(int client, const std::string& name);

      /**
       * Starts the server and begins accepting client connections.
       * @throws ServerException if any error occurs during server startup.
       */
      void run();

      /**
       * Stops the server and closes all client connections.
       * @throws ServerException if any error occurs during server shutdown.
       */
      void stop();

      /**
       * Handles incoming messages from clients and processes them.
       * @throws ServerException if any error occurs during message handling.
       */
      void readFromClients();

      /**
       * Sends messages to clients based on their file descriptors.
       * @throws ServerException if any error occurs during message sending.
       */
      void writeToClients();

      /**
       * Accepts a new client connection and adds it to the list of clients.
       * @param client The file descriptor of the accepted client.
       * @throws ServerException if any error occurs during client acceptance.
       */
      void addClient(int client);

      /**
       * Removes a client from the list of clients and closes the connection.
       * @param client The file descriptor of the client to be removed.
       * @throws ServerException if any error occurs during client removal.
       */
      void removeClient(int client);

      /**
       * Checks if a client is logged in.
       * @param message The message sent by the client.
       */
      void broadcast(const std::string& message);


      /**
       * Sends a message to a specific client.
       * @param client The file descriptor of the client to whom the message will be sent.
       * @param message The message to be sent to the client.
       */
      void sendToClient(int client, const std::string& message);

      /**
       * Sends a message to all clients.
       * @param message The message to be sent to all clients.
       * @param client The file descriptor of the client to whom the message will be sent.
       */
      void commandHelp(int client, const std::string& message);

      /**
       * Sends a message to all clients.
       * @param message The message to be sent to all clients.
       * @param client The file descriptor of the client to whom the message will be sent.
       */
      void commandList(int client, const std::string& message);

      /**
       * Sends a message to all clients.
       * @param message The message to be sent to a specific clien to login
       * @param client The file descriptor of the client to whom the message will be sent.
       */
      void clientLogin(int client, const std::string& message);

      /**
       * Sends a message to all clients.
       * @param message The message to be sent to all clients.
       * @param client The file descriptor of the client to whom the message will be sent.
       */
      void commandsMessage(int client, const std::string& message);

      /**
       * Sends a private message to a specific client. (Not implemented yet)
       * @param req The message to be sent to all clients.
       * @param client The file descriptor of the client to whom the message will be sent.
       */
      void sendPrivateMessage(int client, const std::string &req);

      /**
       * Get the file descriptor of a client by its name.
       * @param name The name of the client.
       */
      int getClientFileDescriptor(const std::string& name);

  private:

      /**
       * Initializes the server socket and sets it to listen for incoming connections.
       * @throws ServerException if any error occurs during socket initialization.
       */
      void _initFdSets();

      /**
       * Parse the incoming message and execute the corresponding command.
       * @param client The file descriptor of the client sending the message.
       * @param message The message sent by the client.
       */
      void _interpretMessage(int client, const std::string& message);

      /**
       * Check if the client is logged in.
       * @param client The file descriptor of the client to be checked.
       * @param message The message sent by the client.
       * @return true if the client is logged in, false otherwise.
       */
      bool _checkIfLoggedIn(int client, const std::string& message);

      std::map<int, int> _clientsPrivateMessagesIndex; // Map to store who sent private messages to whom
      std::map<std::string, void (Server::*)(int, const std::string&)> _commands; // Map to store commands and their corresponding functions
      std::map<int, std::string> _clientsNames; // Map to store client names and their corresponding file descriptors
      std::vector<std::string> _loggedInClients; // Vector to store logged-in clients
      std::vector<int> _clients; // Vector to store client file descriptors
      int _socket; // Socket file descriptor
      int _maxClients; // Maximum number of clients
      int _serverSocket; // Server socket file descriptor
      int _opt; // Socket option value
      unsigned short _port; // Port number on which the server listens
      size_t _maxFd; // Maximum file descriptor value

      fd_set _writeFds; // File descriptor set for writing
      fd_set _readFds; // File descriptor set for reading
      fd_set _exceptFds; // File descriptor set for exceptions

      struct sockaddr_in _serverAddr; // Server address structure
      struct sockaddr_in _clientAddr; // Client address structure
      socklen_t _clientAddrLen; // Length of the client address structure

      bool _running; // Flag to indicate if the server is running
};

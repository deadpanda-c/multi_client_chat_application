# Socket Application with Qt GUI

## Overview

This is a C++ socket application that provides network communication functionality with a graphical interface built using Qt. The application allows users to establish connections, send and receive messages, and manage network sessions efficiently.

The client can be executed on a Windows machine (I think, i don't have windows, but it should work :) ).

For the server, it's already running on [http://51.178.139.114:4242](http://51.178.139.114:4242), but you can still try to use it on your own machine to see the logs (I'm using the `select` syscall, so i think it won't work on Window). 



To generate the documentation of the project, go at the root of the project, and 

## Features

- Client-server communication using sockets
- GUI built with Qt for a user-friendly interface
- Support for TCP protocols
- Real-time message exchange
- Connection status indicators
- Error handling and logging

## Requirements

- C++17 or later
- Qt 6.x
- CMake (if using CMake for build management)
- A compiler that supports C++ (GCC, Clang, or MSVC)

## Installation

### Using CMake

```sh
mkdir build
cd build
cmake ..
make
./socket_app
```

## Usage

1. Launch the application.
2. Enter the server address and port number.
3. Click 'Connect' to establish a connection.
4. Send messages through the interface and view received messages.

### Running Server and Client Separately

#### Running the Server

```sh
./server
```

or

```sh
./server <port>
```

Example:

```sh
./server 8080
```

#### Running the Client

```sh
./client <server_address> <port>
```

Example:

```sh
./socket_client 127.0.0.1 8080
```

## Project Structure

```
.
├── CMakeLists.txt
├── db
├── doc
│   └── Doxyfile
├── include
│   ├── Client.hpp
│   └── Server.hpp
├── lib
│   ├── binary_protocol
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── BinaryProtocol.hpp
│   │   ├── sample
│   │   │   └── main.cpp
│   │   └── src
│   │       └── BinaryProtocol.cpp
│   ├── server_logging
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── Logging.hpp
│   │   └── src
│   │       ├── Logging.cpp
│   │       └── main.cpp
│   └── utils
│       ├── CMakeLists.txt
│       ├── include
│       │   └── Utils.hpp
│       ├── sample
│       │   └── main.cpp
│       └── src
│           └── Utils.cpp
├── LICENSE
└── src
    ├── client
    │   ├── Client.cpp
    │   └── main.cpp
    └── server
        ├── main.cpp
        └── Server.cpp

```



## Client part

The client interface is composed of many part

* A list of the connected users with their username (the username is set by the environment variable: "USER" for Linux machine, and "USERNAME" for Windows machine)
* An area where the chat will be displayed
* A text entry where you can put your message
* And a button to send your message

## License

This project is licensed under the MIT License.

## Contribution

Feel free to contribute by submitting issues or pull requests.

## Contact

For any inquiries, contact me at clement.lagier@epitech.eu

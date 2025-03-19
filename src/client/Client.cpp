#include "Client.hpp"
#include "Logging.hpp"

Client::Client()
{
  _fd = -1;
}

Client::~Client()
{

}

void Client::init(const std::string &ip, unsigned short port)
{
  _ip = ip;
  _port = port;
  _fd = socket(AF_INET, SOCK_STREAM, 0);

  if (_fd == -1)
    throw ClientException(SOCKET_ERROR);

}

void Client::run()
{
  _serv_addr.sin_family = AF_INET;
  _serv_addr.sin_port = htons(_port);
  _serv_addr.sin_addr.s_addr = inet_addr(_ip.c_str());

  if (::connect(_fd, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) < 0)
    throw ClientException(CONNECT_FAILED);
}

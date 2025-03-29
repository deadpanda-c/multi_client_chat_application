#include "Client.hpp"

int main(int ac, char **av)
{
  unsigned short port = (ac > 1) ? std::stoi(av[2]) : 8080;
  std::string ip = (ac > 2) ? av[1] : "127.0.0.1";

  QApplication app(ac, av);
  Client client(ip, port, "My Window");

  client.show();
  return app.exec();
}


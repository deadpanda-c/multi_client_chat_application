#include "Client.hpp"


int main(int ac, char **av) {
  unsigned short port = (ac > 1) ? std::stoi(av[2]) : 8080;
  std::string ip = (ac > 2) ? av[1] : "127.0.0.1";

  try {
    Client client(ip, port);
    client.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}

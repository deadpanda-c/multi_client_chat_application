#include "Client.hpp"

int main(int ac, char **av) {
  unsigned short port = (ac == 3) ? std::stoi(av[2]) : 4242;
  std::string ip = (ac == 3) ? av[1] : "127.0.0.1";
  Client client;

  try {
    client.init(ip, port);
    client.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

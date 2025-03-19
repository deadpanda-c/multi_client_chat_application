#include "Server.hpp"

int main(int ac, char **av) {
  unsigned short port = 8000;

  if (ac != 2) {
      std::cerr << "Usage: ./server <port> (OPTIONAL)" << std::endl;
      return 1;
  }
  try {
    port = std::stoi(av[1]);
  } catch (std::exception &e) {
    std::cerr << "Invalid port number" << std::endl;
    return 1;
  }

  Server server(port);


  try {
    server.init();
    server.run();
    server.stop();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;

}

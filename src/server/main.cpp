#include "Server.hpp"

int main(int ac, char **av)
{
  unsigned short port = (ac == 2) ? std::atoi(av[1]) : 4242;
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

#include "Server.hpp"

int main() {
    Server server(4242);

    try {
      server.init();
      server.run();
      server.stop();
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
    return 0;

}

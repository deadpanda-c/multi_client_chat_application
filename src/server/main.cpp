#include "Server.hpp"

int main() {
    Server server(4242);

    server.init();
    server.run();
    server.stop();
    return 0;
}

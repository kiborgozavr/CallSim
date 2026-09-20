#include "Server.hpp"

int main() {
    constexpr unsigned int DEFAULT_PORT = 15001;
    server::Server server(DEFAULT_PORT);
    server.start();

    return 0;
}

#include "Server.h"

int main() {
    constexpr unsigned int DEFAULT_PORT = 15001;
    Server server(DEFAULT_PORT);
    server.start();

    return 0;
}

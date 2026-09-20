#include "Client.hpp"

int main(int argc, char* argv[]) {
    constexpr unsigned int DEFAULT_PORT = 15001;

    if (argc != 2) {
        std::cerr << "Invalid argument list\n";
        return 1;
    }

    const std::string name = argv[1];

    client::Client client(DEFAULT_PORT, name);
    client.start();
}
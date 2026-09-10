#include "../inc/Client.hpp"

int main() {
    constexpr unsigned int DEFAULT_PORT = 15001;

    client::Client client(DEFAULT_PORT);
    client.start();
}
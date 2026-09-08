#include "Client.h"

#include <boost/asio.hpp>
#include <iostream>

int main() {
    constexpr unsigned int DEFAULT_PORT = 15001;

    Client client(DEFAULT_PORT);

    client.start();

    std::string line;
    while (std::cout << "Type your message: ", std::getline(std::cin, line)) {
        if (line == "quit") {
            client.stop();
            break;
        }
        client.write(line);
    }
}
#include "../inc/Client.hpp"

#include <thread>
#include <iostream>

namespace client {
Client::Client(const unsigned int port) :
        _rwWrapper(port) {
    std::cout << "Client connected!\n";
}
void Client::start() {
    _rwWrapper.asyncRead();
    _rwWrapper.start();

    _consoleThread = std::thread([this] { readFromConsole(); });

    _consoleThread.join();
}
void Client::stop() { _rwWrapper.stop(); }

void Client::write(const std::string& msg) { _rwWrapper.asyncWrite(msg); }

void Client::readFromConsole() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "quit") {
            stop();
            break;
        }
        write(line);
    }
}

}  // namespace client

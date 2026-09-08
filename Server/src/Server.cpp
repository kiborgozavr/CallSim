#include "Server.h"

#include <iostream>

Server::Server(const unsigned int port) :
        rw_wrapper(port) {
    std::cout << "Server started. Waiting for client...\n";
}
void Server::start() { rw_wrapper.start(); }
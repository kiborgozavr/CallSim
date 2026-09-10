#include "../inc/Server.hpp"

#include <iostream>
namespace server {
Server::Server(const unsigned int port) :
        _rwWrapper(port) {
    std::cout << "Server started. Waiting for client...\n";
}
void Server::start() { _rwWrapper.start(); }
}  // namespace server

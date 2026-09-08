#include "Client.h"

Client::Client(const unsigned int port) :
        rw_wrapper(port) {
    std::cout << "Client connected!\n";
}
void Client::start() { rw_wrapper.start(); }
void Client::stop() { rw_wrapper.stop(); }
void Client::write(const std::string& msg) { rw_wrapper.async_write(msg); }
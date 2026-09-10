#pragma once

#include "RwWrapper.hpp"

namespace client {
class Client {
public:
    explicit Client(unsigned int port);

    void start();

private:
    void stop();

    void readFromConsole();

    void write(const std::string &msg);

    std::thread _consoleThread;
    RwWrapper _rwWrapper;
};

}  // namespace client

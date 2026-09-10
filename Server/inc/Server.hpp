#pragma once

#include "RwWrapper.hpp"

namespace server {
class Server {
public:
    explicit Server(unsigned int port);

    void start();

private:
    rwWrapper _rwWrapper;
};

}  // namespace server

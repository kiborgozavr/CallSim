#pragma once

#include "ServerRwWrapper.h"

class Server {
public:
    explicit Server(unsigned int port);

    void start();

private:
    rwWrapperServer rw_wrapper;
};

#pragma once

#include "ClientRwWrapper.h"

#include <iostream>

class Client {
public:
    explicit Client(unsigned int port);

    void start();

    void stop();

    void write(const std::string &msg);

private:
    ClientRwWrapper rw_wrapper;
};

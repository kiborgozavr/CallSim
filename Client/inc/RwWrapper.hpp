#pragma once
#include "Message.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace client {
class RwWrapper {
public:
    explicit RwWrapper(unsigned int port);

    void start();

    void stop();

    void asyncWrite(const CallSim::Message& message);

    void asyncRead();

private:
    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ip::tcp::resolver _resolver;

    boost::asio::streambuf _buffer;

    std::thread _thread;
};

}  // namespace client

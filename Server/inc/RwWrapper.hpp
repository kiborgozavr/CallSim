#pragma once
#include "Message.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace server {
class RwWrapper {
public:
    explicit RwWrapper(unsigned int port);

    void start();

private:
    void acceptClient();

    void asyncRead();

    void asyncReadMessage(std::uint32_t size);

    void asyncWrite(const CallSim::Message& message);

    static void printMessage(const CallSim::Message& message, std::size_t dataSize);

    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _buffer;
};
}  // namespace server

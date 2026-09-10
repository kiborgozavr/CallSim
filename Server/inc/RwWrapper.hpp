#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace server {
class rwWrapper {
public:
    explicit rwWrapper(unsigned int port);

    void start();

private:
    void acceptClient();

    void asyncRead();

    void asyncWrite(const std::string& message);

    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf _buffer;
};
}  // namespace server

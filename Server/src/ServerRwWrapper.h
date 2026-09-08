#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

class rwWrapperServer {
public:
    explicit rwWrapperServer(unsigned int port);

    void start();

private:
    void accept_client();

    void async_read();

    void async_write(const std::string& message);

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf buffer;
};
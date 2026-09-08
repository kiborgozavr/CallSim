#include "ServerRwWrapper.h"

#include <iostream>
#include <memory>

rwWrapperServer::rwWrapperServer(const unsigned int port) :
        acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        socket(io_context) {}
void rwWrapperServer::start() {
    accept_client();
    io_context.run();
}
void rwWrapperServer::accept_client() {
    acceptor.async_accept(socket, [this](const boost::system::error_code& ec) {
        if (ec) {
            std::cerr << "Accept error: " << ec.message() << '\n';
            return;
        }
        std::cout << "client connected!\n";
        async_read();
    });
}
void rwWrapperServer::async_read() {
    boost::asio::async_read_until(
        socket, buffer, '\n', [this](const boost::system::error_code& ec, const std::size_t bytes_transferred) {
            if (ec) {
                std::cerr << "Read error: " << ec.message() << '\n';
                return;
            }

            std::istream stream(&buffer);

            std::string response;
            std::getline(stream, response);
            if (response == "quit") {
                socket.close();
                return;
            }

            std::cout << "Server received " << bytes_transferred << " bytes. Message: " << response << '\n';

            async_write(response);
        });
}
void rwWrapperServer::async_write(const std::string& message) {
    const auto data = std::make_shared<std::string>("Re: " + message + "\n");

    boost::asio::async_write(socket,
                             boost::asio::buffer(*data),
                             [data, this](const boost::system::error_code& ec, const std::size_t bytes_transferred) {
                                 if (ec) {
                                     std::cerr << "Write error: " << ec.message() << '\n';
                                     return;
                                 }
                                 std::cout << "Server sent " << bytes_transferred << " bytes" << '\n';
                                 async_read();
                             });
}
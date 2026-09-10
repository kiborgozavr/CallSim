#include "RwWrapper.hpp"

#include <iostream>
#include <memory>

namespace server {
rwWrapper::rwWrapper(const unsigned int port) :
        _acceptor(_ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        _socket(_ioContext) {}
void rwWrapper::start() {
    acceptClient();
    _ioContext.run();
}
void rwWrapper::acceptClient() {
    _acceptor.async_accept(_socket, [this](const boost::system::error_code& errorCode) {
        if (errorCode) {
            std::cerr << "Accept error: " << errorCode.message() << '\n';
            return;
        }
        std::cout << "client connected!\n";
        asyncRead();
    });
}
void rwWrapper::asyncRead() {
    boost::asio::async_read_until(
        _socket,
        _buffer,
        '\n',
        [this](const boost::system::error_code& errorCode, const std::size_t bytes_transferred) {
            if (errorCode) {
                std::cerr << "Read error: " << errorCode.message() << '\n';
                return;
            }

            std::istream stream(&_buffer);

            std::string response;
            std::getline(stream, response);
            if (response == "quit") {
                _socket.close();
                return;
            }

            std::cout << "Server received " << bytes_transferred << " bytes. Message: " << response << '\n';

            asyncWrite(response);
        });
}
void rwWrapper::asyncWrite(const std::string& message) {
    const auto data = std::make_shared<std::string>("Re: " + message + "\n");

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(*data),
        [data, this](const boost::system::error_code& errorCode, const std::size_t bytes_transferred) {
            if (errorCode) {
                std::cerr << "Write error: " << errorCode.message() << '\n';
                return;
            }
            std::cout << "Server sent " << bytes_transferred << " bytes" << '\n';
            asyncRead();
        });
}
}  // namespace server

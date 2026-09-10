#include "../inc/RwWrapper.hpp"

#include <iostream>
#include <thread>

namespace client {
RwWrapper::RwWrapper(const unsigned int port) :
        _socket(_ioContext),
        _resolver(_ioContext) {
    const auto endpoint = _resolver.resolve("localhost", std::to_string(port));
    boost::asio::connect(_socket, endpoint);
}
void RwWrapper::start() {
    _thread = std::thread([this] { _ioContext.run(); });
}
void RwWrapper::stop() {
    _socket.close();

    if (_thread.joinable()) {
        _thread.join();
    }
}
void RwWrapper::asyncWrite(const std::string& message) {
    auto data = std::make_shared<std::string>(message + "\n");

    boost::asio::async_write(
        _socket, boost::asio::buffer(*data), [this, data](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                std::cerr << "Write error: " << errorCode.message() << '\n';
            }
        });
}
void RwWrapper::asyncRead() {
    boost::asio::async_read_until(
        _socket, _buffer, '\n', [this](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                if (errorCode == boost::asio::error::operation_aborted) {
                    return;
                }

                std::cerr << "Read error: " << errorCode.message() << '\n';
                return;
            }

            std::istream stream(&_buffer);

            std::string response;
            std::getline(stream, response);

            std::cout << response << '\n';

            asyncRead();
        });
}
}  // namespace client

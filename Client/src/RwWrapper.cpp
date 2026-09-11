#include "../inc/RwWrapper.hpp"
#include "Message.pb.h"

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
void RwWrapper::asyncWrite(const CallSim::Message& message) {
    std::string data;

    if (!message.SerializeToString(&data)) {
        std::cerr << "Failed to serialize message\n";
        return;
    }

    const auto size = static_cast<std::uint32_t>(data.size());

    auto buffer = std::make_shared<std::string>(sizeof(size) + data.size(), '\0');

    std::copy_n(reinterpret_cast<const char*>(&size), sizeof(size), buffer->begin());

    std::copy(data.begin(), data.end(), buffer->begin() + sizeof(size));

    boost::asio::async_write(
        _socket, boost::asio::buffer(*buffer), [buffer](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                std::cerr << "Write error: " << errorCode.message() << '\n';
            }
        });
}
void RwWrapper::asyncRead() {
    auto sizeBuffer = std::make_shared<std::array<char, sizeof(std::uint32_t)>>();

    boost::asio::async_read(_socket,
                            boost::asio::buffer(*sizeBuffer),
                            [this, sizeBuffer](const boost::system::error_code& errorCode, std::size_t) {
                                if (errorCode) {
                                    std::cerr << "Read size error: " << errorCode.message() << '\n';
                                    return;
                                }

                                std::uint32_t size;
                                std::copy(sizeBuffer->begin(), sizeBuffer->end(), reinterpret_cast<char*>(&size));

                                auto data = std::make_shared<std::string>(size, '\0');

                                boost::asio::async_read(
                                    _socket,
                                    boost::asio::buffer(*data),
                                    [this, data](const boost::system::error_code& errorCode, std::size_t) {
                                        if (errorCode) {
                                            std::cerr << "Read message error: " << errorCode.message() << '\n';
                                            return;
                                        }

                                        CallSim::Message message;
                                        if (!message.ParseFromArray(data->data(), data->size())) {
                                            std::cerr << "Failed to parse protobuf message\n";
                                            return;
                                        }

                                        std::cout << message.payload() << '\n';

                                        asyncRead();
                                    });
                            });
}
}  // namespace client

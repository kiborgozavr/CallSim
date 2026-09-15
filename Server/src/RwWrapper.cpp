#include "Message.pb.h"
#include "RwWrapper.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>

namespace server {
RwWrapper::RwWrapper(const unsigned int port) :
        _acceptor(_ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        _socket(_ioContext) {}
void RwWrapper::start() {
    acceptClient();
    _ioContext.run();
}
void RwWrapper::acceptClient() {
    _acceptor.async_accept(_socket, [this](const boost::system::error_code& errorCode) {
        if (errorCode) {
            std::cerr << "Accept error: " << errorCode.message() << '\n';
            return;
        }
        std::cout << "client connected!\n";
        asyncRead();
    });
}
void RwWrapper::asyncRead() {
    auto size = std::make_shared<std::uint32_t>();

    boost::asio::async_read(_socket,
                            boost::asio::buffer(size.get(), sizeof(*size)),
                            [this, size](const boost::system::error_code& errorCode, std::size_t) {
                                if (errorCode) {
                                    std::cerr << "Read size error: " << errorCode.message() << '\n';
                                    return;
                                }

                                asyncReadMessage(*size);
                            });
}
void RwWrapper::asyncReadMessage(const std::uint32_t size) {
    auto data = std::make_shared<std::string>(size, '\0');

    boost::asio::async_read(
        _socket, boost::asio::buffer(*data), [this, data](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                std::cerr << "Read message error: " << errorCode.message() << '\n';
                return;
            }

            CallSim::Message message;

            if (!message.ParseFromArray(data->data(), data->size())) {
                std::cerr << "Failed to parse protobuf message\n";
                return;
            }

            if (message.payload() == "quit") {
                std::cout << "Client " << message.from_user() << " disconnected\n";
                return;
            }

            printMessage(message, data->size());

            message.set_payload("Re: " + message.payload());
            asyncWrite(message);
        });
}
void RwWrapper::asyncWrite(const CallSim::Message& message) {
    std::string data;

    if (!message.SerializeToString(&data)) {
        std::cerr << "Failed to serialize message\n";
        return;
    }

    const auto size = static_cast<std::uint32_t>(data.size());
    auto buffer = std::make_shared<std::string>(sizeof(size) + data.size(), '\0');

    std::memcpy(buffer->data(), &size, sizeof(size));
    std::memcpy(buffer->data() + sizeof(size), data.data(), data.size());

    boost::asio::async_write(_socket,
                             boost::asio::buffer(*buffer),
                             [this, buffer, data](const boost::system::error_code& errorCode, std::size_t) {
                                 if (errorCode) {
                                     std::cerr << "Write error: " << errorCode.message() << '\n';
                                     return;
                                 }
                                 std::cout << "Server sent " << data.size() << " bytes" << '\n';
                                 asyncRead();
                             });
}
void RwWrapper::printMessage(const CallSim::Message& message, const std::size_t dataSize) {
    std::cout << "Server received " << dataSize << " bytes. Type of message: " << MessageType_Name(message.type())
              << ". Timestamp: ";

    const auto time = std::chrono::system_clock::time_point{std::chrono::milliseconds(message.timestamp())};

    const std::time_t time_t_value = std::chrono::system_clock::to_time_t(time);

    std::cout << std::put_time(std::localtime(&time_t_value), "%Y-%m-%d %H:%M:%S") << '\n';
}
}  // namespace server

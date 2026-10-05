#include "Client.hpp"
#include "Message.pb.h"
#include "RwWrapper.hpp"

#include <cstring>
#include <iostream>
#include <thread>

namespace client {
RwWrapper::RwWrapper(Client& client, const unsigned int port) :
        _socket(_ioContext),
        _resolver(_ioContext),
        _client(client) {
    const auto endpoint = _resolver.resolve("localhost", std::to_string(port));
    boost::asio::connect(_socket, endpoint);
}
void RwWrapper::start() {
    asyncRead();
    _thread = std::thread([this] { _ioContext.run(); });
}
void RwWrapper::stop() {
    _socket.close();

    if (_thread.joinable()) {
        _thread.join();
    }
}
void RwWrapper::asyncWrite(CallSim::Message message) {
    std::string data;

    if (!message.SerializeToString(&data)) {
        std::cerr << "Failed to serialize message\n";
        return;
    }

    const auto size = static_cast<std::uint32_t>(data.size());

    auto buffer = std::make_shared<std::string>(sizeof(size) + data.size(), '\0');

    std::memcpy(buffer->data(), &size, sizeof(size));
    std::memcpy(buffer->data() + sizeof(size), data.data(), data.size());
    boost::asio::async_write(
        _socket, boost::asio::buffer(*buffer), [buffer, this](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                std::cerr << "Write error: " << errorCode.message() << '\n';
            }
        });
}
void RwWrapper::asyncRead() {
    auto size = std::make_shared<std::uint32_t>();

    boost::asio::async_read(_socket,
                            boost::asio::buffer(size.get(), sizeof(*size)),
                            [this, size](const boost::system::error_code& errorCode, std::size_t) {
                                if (errorCode) {
                                    if (errorCode == boost::asio::error::operation_aborted) {
                                        return;
                                    }

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
                if (errorCode == boost::asio::error::operation_aborted) {
                    return;
                }
                std::cerr << "Read message error: " << errorCode.message() << '\n';
                return;
            }

            CallSim::Message message;

            if (!message.ParseFromArray(data->data(), data->size())) {
                std::cerr << "Failed to parse protobuf message\n";
                return;
            }
            _client.handleMessage(message);
            asyncRead();
        });
}
}  // namespace client

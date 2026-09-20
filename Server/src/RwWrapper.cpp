#include "Message.pb.h"
#include "RwWrapper.hpp"
#include "Server.hpp"

#include <cstring>
#include <iostream>
#include <memory>

namespace server {
using tcp = boost::asio::ip::tcp;

RwWrapper::RwWrapper(Server& server, const unsigned int port) :
        _acceptor(_ioContext, tcp::endpoint(tcp::v4(), port)),
        _server(server) {}

void RwWrapper::start() {
    acceptClient();
    _ioContext.run();
}

void RwWrapper::acceptClient() {
    auto socket = std::make_shared<tcp::socket>(_ioContext);

    _acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& errorCode) {
        if (errorCode) {
            std::cerr << "Accept error: " << errorCode.message() << '\n';
            acceptClient();
            return;
        }
        const unsigned int clientId = _server.saveClient(socket);

        acceptClient();
        asyncRead(socket, clientId);
    });
}
void RwWrapper::asyncRead(std::shared_ptr<tcp::socket> socket, unsigned int clientId) {
    auto size = std::make_shared<std::uint32_t>();

    boost::asio::async_read(*socket,
                            boost::asio::buffer(size.get(), sizeof(*size)),
                            [this, size, socket, clientId](const boost::system::error_code& errorCode, std::size_t) {
                                if (errorCode) {
                                    std::cerr << "Read size error: " << errorCode.message() << '\n';
                                    asyncRead(socket, clientId);
                                    return;
                                }
                                asyncReadMessage(socket, *size, clientId);
                            });
}
void RwWrapper::asyncReadMessage(std::shared_ptr<tcp::socket> socket, const std::uint32_t size, unsigned int clientId) {
    auto data = std::make_shared<std::string>(size, '\0');

    boost::asio::async_read(*socket,
                            boost::asio::buffer(*data),
                            [this, data, socket, clientId](const boost::system::error_code& errorCode, std::size_t) {
                                if (errorCode) {
                                    std::cerr << "Read message error: " << errorCode.message() << '\n';
                                    asyncRead(socket, clientId);
                                    return;
                                }

                                CallSim::Message message;

                                if (!message.ParseFromArray(data->data(), data->size())) {
                                    std::cerr << "Failed to parse protobuf message\n";
                                    return;
                                }

                                const CallSim::Message responseMessage = _server.handleMessage(message, clientId);
                                asyncWrite(socket, responseMessage, clientId);
                            });
}
void RwWrapper::asyncWrite(std::shared_ptr<tcp::socket> socket, const CallSim::Message message, unsigned int clientId) {
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
        *socket,
        boost::asio::buffer(*buffer),
        [this, buffer, data, socket, clientId](const boost::system::error_code& errorCode, std::size_t) {
            if (errorCode) {
                std::cerr << "Write error: " << errorCode.message() << '\n';
                return;
            }
            std::cout << "Server sent " << data.size() << " bytes" << '\n';
            asyncRead(socket, clientId);
        });
}
}  // namespace server

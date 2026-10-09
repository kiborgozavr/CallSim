#pragma once

#include "IRwWrapper.hpp"
#include "Message.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace client {
class Client;
using tcp = boost::asio::ip::tcp;

class RwWrapper final : public IRwWrapper {
public:
    explicit RwWrapper(Client& client, unsigned int port);

    void start() override;

    void stop() override;

    void asyncWrite(CallSim::Message message) override;

    void asyncRead() override;

private:
    void asyncReadMessage(std::uint32_t size);

    boost::asio::io_context _ioContext;
    tcp::socket _socket;
    tcp::resolver _resolver;

    Client& _client;

    std::thread _thread;
};

}  // namespace client

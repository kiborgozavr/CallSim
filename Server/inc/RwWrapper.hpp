#pragma once
#include "Message.pb.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace server {
using tcp = boost::asio::ip::tcp;

class Server;
class SavedClient;
class RwWrapper {
public:
    explicit RwWrapper(Server& server, unsigned int port);

    void start();

    void putTaskInContext(SavedClient& client, const CallSim::Message& message,
                          std::function<void(SavedClient&, const CallSim::Message&)> function);

    void asyncWrite(std::shared_ptr<tcp::socket> socket, const CallSim::Message& message, unsigned int clientId);

private:
    void acceptClient();

    void asyncRead(std::shared_ptr<tcp::socket> socket, unsigned int clientId);

    void asyncReadMessage(std::shared_ptr<tcp::socket> socket, std::uint32_t size, unsigned int clientId);

    boost::asio::io_context _ioContext;
    tcp::acceptor _acceptor;
    Server& _server;
};
}  // namespace server

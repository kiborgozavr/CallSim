#pragma once

#include "ClientStates.hpp"
#include "RwWrapper.hpp"

namespace server {
using tcp = boost::asio::ip::tcp;

struct SavedClient {
    std::shared_ptr<tcp::socket> _socket;
    std::string _name;
    common::ClientStates _state = common::ClientStates::connected;

    explicit SavedClient(const std::shared_ptr<tcp::socket>& socket) :
            _socket(socket) {}
};

class Server {
public:
    explicit Server(unsigned int port);

    void start();

    CallSim::Message handleMessage(const CallSim::Message& message, unsigned int clientId);

    unsigned int saveClient(std::shared_ptr<tcp::socket> socket);

private:
    CallSim::Message registerClient(const CallSim::Message& message, unsigned int clientId);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& msgPayload);
    CallSim::Message registerCall(const CallSim::Message& message, unsigned int clientId);

    CallSim::Message createMessage(CallSim::MessageType msgType, std::string msgPayload);
    CallSim::Message createMessage(CallSim::MessageType msgType);
    void setTimestamp(CallSim::Message& message);

    void printMessage(const CallSim::Message& message, std::size_t dataSize);

    RwWrapper _rwWrapper;
    std::unordered_map<unsigned int, SavedClient> _clients;
    std::unordered_set<std::string> _registeredClientNames;
    unsigned int _nextClientId = 0;
};

}  // namespace server

#pragma once

#include "ClientStates.hpp"
#include "RwWrapper.hpp"

#include <optional>

namespace server {
using tcp = boost::asio::ip::tcp;

struct SavedClient {
    std::shared_ptr<tcp::socket> _socket;
    unsigned int _clientId;
    std::string _name;
    common::ClientStates _state = common::ClientStates::connected;

    explicit SavedClient(const std::shared_ptr<tcp::socket>& socket) :
            _socket(socket) {}
};

class Server {
public:
    explicit Server(unsigned int port);

    void start();

    void handleMessage(const CallSim::Message& message, unsigned int clientId);

    unsigned int saveClient(std::shared_ptr<tcp::socket> socket);

    void checkAddresseePresence(SavedClient& client, const CallSim::Message& message);

private:
    void registerClient(SavedClient& client, const CallSim::Message& message, unsigned int clientId);

    void registerCall(SavedClient& client, const CallSim::Message& message, unsigned int clientId);

    void handleRejectCall(const SavedClient& client, const CallSim::Message& message, unsigned int clientId);
    void handleAcceptCall(const SavedClient& client, const CallSim::Message& message, unsigned int clientId);

    std::optional<std::reference_wrapper<SavedClient>> findClient(unsigned int clientId);
    std::optional<std::reference_wrapper<SavedClient>> findClient(const std::string& clientName);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& msgPayload = "",
                                   const std::string& fromUser = "");
    void setTimestamp(CallSim::Message& message);

    RwWrapper _rwWrapper;
    std::map<std::pair<common::ClientStates, CallSim::MessageType>,
             std::pair<common::ClientStates, std::function<void(SavedClient&, const CallSim::Message&, unsigned int)>>>
        _transitionTable;
    std::unordered_map<unsigned int, SavedClient> _clients;
    std::unordered_set<std::string> _registeredClientNames;
    unsigned int _nextClientId = 0;
};

}  // namespace server

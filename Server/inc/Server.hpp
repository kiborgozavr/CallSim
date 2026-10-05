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
    void handleMessage(SavedClient& client, const CallSim::Message& message);

    unsigned int saveClient(std::shared_ptr<tcp::socket> socket);

    void checkAddressee(SavedClient& client, const CallSim::Message& message);

private:
    void registerClient(SavedClient& client, const CallSim::Message& message);

    void registerCall(SavedClient& client, const CallSim::Message& message);

    void handleRejectCall(const SavedClient& client, const CallSim::Message& message);
    void handleAcceptCall(const SavedClient& client, const CallSim::Message& message);
    void handleFinishCall(const SavedClient& client, const CallSim::Message& message);
    void resendMessage(const SavedClient& client, const CallSim::Message& message);

    std::optional<std::reference_wrapper<SavedClient>> findClient(unsigned int clientId);
    std::optional<std::reference_wrapper<SavedClient>> findClient(const std::string& clientName);

    bool isAddresseeInRegisteredState(const SavedClient& addressee);

    void writeMessage(SavedClient& client, const CallSim::Message& message);

    CallSim::Message createMessage(CallSim::MessageType msgType, const std::string& msgPayload = "",
                                   const std::string& fromUser = "");
    void printTimestamp(const CallSim::Message& message);
    void printTimestamp(const std::chrono::system_clock::time_point& timePoint);
    void printLog(const SavedClient& client, const CallSim::Message& message);
    void printLog(const SavedClient& client);

    RwWrapper _rwWrapper;
    std::map<std::pair<common::ClientStates, CallSim::MessageType>,
             std::pair<common::ClientStates, std::function<void(SavedClient&, const CallSim::Message&)>>>
        _transitionTable;
    std::unordered_map<unsigned int, SavedClient> _clients;
    std::unordered_set<std::string> _registeredClientNames;
    unsigned int _nextClientId = 0;
};

}  // namespace server

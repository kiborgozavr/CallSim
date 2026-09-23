#include "ClientStates.hpp"
#include "Server.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

namespace server {
Server::Server(const unsigned int port) :
        _rwWrapper(*this, port) {
    _transitionTable.emplace(std::make_pair(common::ClientStates::connected, CallSim::MessageType::Register),
                             std::make_pair(common::ClientStates::registered,
                                            [this](const CallSim::Message& message, const unsigned int clientId) {
                                                return registerClient(message, clientId);
                                            }));

    _transitionTable.emplace(std::make_pair(common::ClientStates::registered, CallSim::MessageType::Call),
                             std::make_pair(common::ClientStates::calling,
                                            [this](const CallSim::Message& message, const unsigned int clientId) {
                                                return registerCall(message, clientId);
                                            }));

    std::cout << "Server started. Waiting for client...\n";
}
void Server::start() { _rwWrapper.start(); }

CallSim::Message Server::handleMessage(const CallSim::Message& message, const unsigned int clientId) {
    auto clientIt = _clients.find(clientId);

    if (clientIt == _clients.end()) {
        return createMessage(CallSim::MessageType::Error, "Client not found!");
    }

    SavedClient& client = clientIt->second;

    common::ClientStates state = client._state;
    CallSim::MessageType type = message.type();

    auto transitionIt = _transitionTable.find({state, type});

    if (transitionIt == _transitionTable.end()) {
        return createMessage(CallSim::MessageType::Error, "Unknown transition!");
    }

    const auto& transition = transitionIt->second;
    const common::ClientStates nextState = transition.first;
    const auto& handler = transition.second;

    CallSim::Message response = handler(message, clientId);

    client._state = nextState;

    return response;
}

unsigned int Server::saveClient(std::shared_ptr<tcp::socket> socket) {
    unsigned int clientId = _nextClientId;
    _nextClientId++;

    _clients.emplace(clientId, SavedClient(socket));
    std::cout << "New client accepted. Client get ID = " << clientId << "\n";

    return clientId;
}

CallSim::Message Server::registerClient(const CallSim::Message& message, const unsigned int clientId) {
    const std::string& ClientNameFromMessage = message.from_user();

    if (_registeredClientNames.find(ClientNameFromMessage) != _registeredClientNames.end()) {
        return createMessage(CallSim::MessageType::Error,
                             "This username is already taken. Restart the application with a different name.");
    }

    SavedClient& client = _clients.at(clientId);
    client._state = common::ClientStates::registered;
    client._name = ClientNameFromMessage;
    _registeredClientNames.insert(ClientNameFromMessage);

    std::cout << "Client with ID = " << clientId << " registered with name " << client._name << "\n";

    return createMessage(CallSim::MessageType::Registered);
}
CallSim::Message Server::registerCall(const CallSim::Message& message, const unsigned int clientId) {
    SavedClient& client = _clients.at(clientId);
    client._state = common::ClientStates::calling;

    std::cout << "Client with ID = " << clientId << " calling " << message.payload() << "\n";

    return createMessage(CallSim::MessageType::Calling);
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload) {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_payload(msgPayload);
    setTimestamp(message);

    return message;
}

void Server::setTimestamp(CallSim::Message& message) {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    message.set_timestamp(timestamp);
}

}  // namespace server

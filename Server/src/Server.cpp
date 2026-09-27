#include "ClientStates.hpp"
#include "Server.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

namespace server {
Server::Server(const unsigned int port) :
        _rwWrapper(*this, port) {
    _transitionTable.emplace(
        std::make_pair(common::ClientStates::connected, CallSim::MessageType::Register),
        std::make_pair(common::ClientStates::registered,
                       [this](SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
                           return registerClient(client, message, clientId);
                       }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::registered, CallSim::MessageType::Call),
        std::make_pair(common::ClientStates::calling,
                       [this](SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
                           registerCall(client, message, clientId);
                       }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::answering, CallSim::MessageType::Reject),
        std::make_pair(common::ClientStates::registered,
                       [this](SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
                           handleRejectCall(client, message, clientId);
                       }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::answering, CallSim::MessageType::Accept),
        std::make_pair(common::ClientStates::talking,
                       [this](SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
                           handleAcceptCall(client, message, clientId);
                       }));

    std::cout << "Server started. Waiting for client...\n";
}
void Server::start() { _rwWrapper.start(); }

unsigned int Server::saveClient(std::shared_ptr<tcp::socket> socket) {
    unsigned int clientId = _nextClientId;
    SavedClient client{socket};
    client._clientId = _nextClientId;
    _nextClientId++;

    _clients.emplace(clientId, client);
    std::cout << "New client accepted. Client get ID = " << clientId << "\n";

    return clientId;
}

void Server::handleMessage(const CallSim::Message& message, const unsigned int clientId) {
    const auto clientOptional = findClient(clientId);
    if (!clientOptional.has_value()) {
        std::cerr << "Client with ID = " << clientId << " not found";
    }
    SavedClient& client = clientOptional->get();

    const auto transitionIterator = _transitionTable.find({client._state, message.type()});
    if (transitionIterator == _transitionTable.end()) {
        std::cerr << "Unknown transition";
        return;
    }

    const auto& [fst, snd] = transitionIterator->second;
    const common::ClientStates nextState = fst;
    const auto& handler = snd;

    client._state = nextState;
    handler(client, message, clientId);
}

void Server::checkAddresseePresence(SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    if (addresseeName == message.from_user()) {
        _rwWrapper.asyncWrite(client._socket,
                              createMessage(CallSim::MessageType::Rejected, "You cannot call yourself\n"),
                              client._clientId);
        return;
    }

    const auto addresseeOptional = findClient(addresseeName);
    if (!addresseeOptional) {
        std::cout << "Client with name " << addresseeName << " not found\n";
        _rwWrapper.asyncWrite(
            client._socket,
            createMessage(CallSim::MessageType::Rejected, "Addressee with such name is not registered\n"),
            client._clientId);
        return;
    }

    SavedClient& addressee = addresseeOptional->get();
    addressee._state = common::ClientStates::answering;

    std::cout << "Client with ID = " << addressee._clientId << " is answering\n";
    _rwWrapper.asyncWrite(
        addressee._socket, createMessage(CallSim::MessageType::Answering, "", client._name), addressee._clientId);
}

void Server::registerClient(SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
    const std::string& ClientNameFromMessage = message.from_user();

    if (_registeredClientNames.find(ClientNameFromMessage) != _registeredClientNames.end()) {
        _rwWrapper.asyncWrite(
            client._socket,
            createMessage(CallSim::MessageType::Error,
                          "This username is already taken. Restart the application with a different name.\n"),
            clientId);
        return;
    }

    client._state = common::ClientStates::registered;
    client._name = ClientNameFromMessage;
    _registeredClientNames.insert(ClientNameFromMessage);

    std::cout << "Client with ID = " << clientId << " registered with name " << client._name << "\n";

    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::MessageType::Registered), clientId);
}

void Server::registerCall(SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
    client._state = common::ClientStates::calling;

    std::cout << "Client with ID = " << clientId << " is calling\n";

    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::MessageType::Calling), clientId);

    _rwWrapper.putTaskInContext(client, message, [this](SavedClient& client, const CallSim::Message& message) {
        checkAddresseePresence(client, message);
    });
}

void Server::handleRejectCall(const SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
    const std::string& addresseeName = message.to_user();
    const auto addresseeOptional = findClient(addresseeName);

    if (!addresseeOptional) {
        std::cout << "Client with name " << addresseeName << " is not found\n";
        return;
    }

    SavedClient& addressee = addresseeOptional->get();

    addressee._state = common::ClientStates::registered;
    _rwWrapper.asyncWrite(addressee._socket, createMessage(CallSim::Rejected), addressee._clientId);
    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::Rejected), clientId);

    std::cout << "Client with ID = " << clientId << " is registered\n";
    std::cout << "Client with ID = " << addressee._clientId << " is registered\n";
}

void Server::handleAcceptCall(const SavedClient& client, const CallSim::Message& message, const unsigned int clientId) {
    const std::string& addresseeName = message.to_user();
    const auto addresseeOptional = findClient(addresseeName);

    if (!addresseeOptional) {
        std::cout << "Client with name " << addresseeName << " is not found\n";
        return;
    }

    SavedClient& addressee = addresseeOptional->get();

    addressee._state = common::ClientStates::talking;
    _rwWrapper.asyncWrite(addressee._socket, createMessage(CallSim::Accepted, "", client._name), addressee._clientId);
    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::Accepted, "", addresseeName), clientId);

    std::cout << "Client with ID = " << clientId << " is talking\n";
    std::cout << "Client with ID = " << addressee._clientId << " is talking\n";
}

std::optional<std::reference_wrapper<SavedClient>> Server::findClient(const unsigned int clientId) {
    const auto clientIterator = _clients.find(clientId);

    if (clientIterator == _clients.end()) {
        return std::nullopt;
    }

    return clientIterator->second;
}

std::optional<std::reference_wrapper<SavedClient>> Server::findClient(const std::string& clientName) {
    for (auto& [clientId, client] : _clients) {
        if (client._name == clientName) {
            return client;
        }
    }
    return std::nullopt;
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload,
                                       const std::string& fromUser) {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_payload(msgPayload);
    message.set_from_user(fromUser);
    setTimestamp(message);

    return message;
}

void Server::setTimestamp(CallSim::Message& message) {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    message.set_timestamp(timestamp);
}

}  // namespace server

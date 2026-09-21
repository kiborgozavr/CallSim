#include "ClientStates.hpp"
#include "Server.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace server {
Server::Server(const unsigned int port) :
        _rwWrapper(*this, port) {
    std::cout << "Server started. Waiting for client...\n";
}
void Server::start() { _rwWrapper.start(); }

CallSim::Message Server::handleMessage(const CallSim::Message& message, const unsigned int clientId) {
    switch (message.type()) {
        case CallSim::MessageType::Register:
            return registerClient(message, clientId);
    }
}
unsigned int Server::saveClient(std::shared_ptr<tcp::socket> socket) {
    unsigned int clientId = _nextClientId;
    _nextClientId++;

    _clients.emplace(clientId, SavedClient(socket));
    std::cout << "New client accepted. Client get ID = " << clientId << "\n";

    return clientId;
}

CallSim::Message Server::registerClient(const CallSim::Message& message, const unsigned int clientId) {
    if (_clients.find(clientId) == _clients.end()) {
        return createMessage(CallSim::MessageType::Text);  // TODO - change type to err (will be in the next PR)
    }

    SavedClient& client = _clients.at(clientId);
    client._name = message.from_user();
    client._state = common::ClientStates::registered;

    std::cout << "Client with ID = " << clientId << " registered with name " << client._name << "\n";

    return createMessage(CallSim::MessageType::Registered);
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload) {
    auto message = createMessage(msgType);

    message.set_payload(msgPayload);
    setTimestamp(message);

    return message;
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType) {
    CallSim::Message message;

    message.set_type(msgType);
    setTimestamp(message);

    return message;
}

void Server::setTimestamp(CallSim::Message& message) {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    message.set_timestamp(timestamp);
}

void Server::printMessage(const CallSim::Message& message, const std::size_t dataSize) {
    std::cout << "Server received " << dataSize << " bytes. Type of message: " << MessageType_Name(message.type())
              << ". Timestamp: ";

    const auto time = std::chrono::system_clock::time_point{std::chrono::milliseconds(message.timestamp())};
    const std::time_t time_t_value = std::chrono::system_clock::to_time_t(time);
    std::cout << std::put_time(std::localtime(&time_t_value), "%Y-%m-%d %H:%M:%S") << '\n';
}
}  // namespace server

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
        std::make_pair(common::ClientStates::registered, [this](SavedClient& client, const CallSim::Message& message) {
            return registerClient(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::registered, CallSim::MessageType::Call),
        std::make_pair(common::ClientStates::calling, [this](SavedClient& client, const CallSim::Message& message) {
            registerCall(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::registered, CallSim::MessageType::Answering),
        std::make_pair(common::ClientStates::answering, [this](SavedClient& client, const CallSim::Message& message) {
            writeMessage(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::calling, CallSim::MessageType::Rejected),
        std::make_pair(common::ClientStates::registered, [this](SavedClient& client, const CallSim::Message& message) {
            writeMessage(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::calling, CallSim::MessageType::Accepted),
        std::make_pair(common::ClientStates::talking, [this](SavedClient& client, const CallSim::Message& message) {
            writeMessage(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::answering, CallSim::MessageType::Reject),
        std::make_pair(common::ClientStates::registered, [this](SavedClient& client, const CallSim::Message& message) {
            handleRejectCall(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::answering, CallSim::MessageType::Accept),
        std::make_pair(common::ClientStates::talking, [this](SavedClient& client, const CallSim::Message& message) {
            handleAcceptCall(client, message);
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
    printTimestamp(std::chrono::system_clock::now());
    std::cout << "New client accepted. Client get ID = " << clientId << "\n";

    return clientId;
}

void Server::handleMessage(const CallSim::Message& message, const unsigned int clientId) {
    const auto clientOptional = findClient(clientId);
    if (!clientOptional.has_value()) {
        printTimestamp(std::chrono::system_clock::now());
        std::cerr << "Client with ID = " << clientId << " not found";
        return;
    }

    SavedClient& client = clientOptional->get();
    handleMessage(client, message);
}

void Server::handleMessage(SavedClient& client, const CallSim::Message& message) {
    const auto transitionIterator = _transitionTable.find({client._state, message.type()});
    if (transitionIterator == _transitionTable.end()) {
        printTimestamp(std::chrono::system_clock::now());
        std::cout << "Unknown transition from " << client._name << "\n";
        return;
    }

    const auto& [nextState, handler] = transitionIterator->second;

    client._state = nextState;
    printLog(client, message);

    handler(client, message);
}

void Server::checkAddresseePresence(SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    if (addresseeName == message.from_user()) {
        _rwWrapper.putTaskInContext(
            client, createMessage(CallSim::MessageType::Rejected, "You cannot call yourself\n"), [this](SavedClient& client, const CallSim::Message& responseMsg) {
                handleMessage(client, responseMsg);
            });
        return;
    }

    const auto addresseeOptional = findClient(addresseeName);
    if (!addresseeOptional) {
        printTimestamp(std::chrono::system_clock::now());
        std::cout << "Client with name " << addresseeName << " not found\n";

        auto responseMsg =
            createMessage(CallSim::MessageType::Rejected, "Addressee with such name is not registered\n");
        _rwWrapper.putTaskInContext(
            client, responseMsg, [this](SavedClient& client, const CallSim::Message& responseMsg) {
                handleMessage(client, responseMsg);
            });
        return;
    }

    SavedClient& addressee = addresseeOptional->get();
    const auto responseMsg = createMessage(CallSim::MessageType::Answering, "", client._name);

    _rwWrapper.putTaskInContext(
        addressee, responseMsg, [this](SavedClient& addressee, const CallSim::Message& responseMsg) {
            handleMessage(addressee, responseMsg);
        });
}

void Server::registerClient(SavedClient& client, const CallSim::Message& message) {
    const std::string& ClientNameFromMessage = message.from_user();

    if (_registeredClientNames.find(ClientNameFromMessage) != _registeredClientNames.end()) {
        _rwWrapper.asyncWrite(
            client._socket,
            createMessage(CallSim::MessageType::Error,
                          "This username is already taken. Restart the application with a different name.\n"),
            client._clientId);
        return;
    }

    client._name = ClientNameFromMessage;
    _registeredClientNames.insert(ClientNameFromMessage);

    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::MessageType::Registered), client._clientId);
}

void Server::registerCall(SavedClient& client, const CallSim::Message& message) {
    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::MessageType::Calling), client._clientId);

    _rwWrapper.putTaskInContext(client, message, [this](SavedClient& client, const CallSim::Message& message) {
        checkAddresseePresence(client, message);
    });
}

void Server::handleRejectCall(const SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    const auto addresseeOptional = findClient(addresseeName);

    if (!addresseeOptional) {
        printTimestamp(std::chrono::system_clock::now());
        std::cout << "Client with name " << addresseeName << " is not found\n";
        return;
    }

    SavedClient& addressee = addresseeOptional->get();

    _rwWrapper.putTaskInContext(
        addressee,  createMessage(CallSim::Rejected, "The call was rejected"), [this](SavedClient& addressee, const CallSim::Message& responseMsg) {
            handleMessage(addressee, responseMsg);
        });

    _rwWrapper.asyncWrite(client._socket, createMessage(CallSim::Rejected, "The call was rejected"), client._clientId);
}

void Server::handleAcceptCall(const SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    const auto addresseeOptional = findClient(addresseeName);

    if (!addresseeOptional) {
        printTimestamp(std::chrono::system_clock::now());
        std::cout << "Client with name " << addresseeName << " is not found\n";
        return;
    }

    SavedClient& addressee = addresseeOptional->get();

    _rwWrapper.putTaskInContext(addressee,
                                createMessage(CallSim::Accepted, "", client._name),
                                [this](SavedClient& addressee, const CallSim::Message& responseMsg) {handleMessage(addressee, responseMsg);});
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

void Server::writeMessage(SavedClient& client, const CallSim::Message& message) {
    _rwWrapper.asyncWrite(client._socket, message, client._clientId);
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload,
                                       const std::string& fromUser) {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_payload(msgPayload);
    message.set_from_user(fromUser);

    return message;
}

void Server::printTimestamp(const CallSim::Message& message) {
    const auto timePoint = std::chrono::system_clock::time_point{std::chrono::milliseconds(message.timestamp())};
    printTimestamp(timePoint);
}

void Server::printTimestamp(const std::chrono::system_clock::time_point& timePoint) {
    const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);

    std::tm localTime{};
    localtime_r(&time, &localTime);

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()).count() % 1000;

    std::cout << std::put_time(&localTime, "[%Y/%m/%d %H:%M:%S.") << std::setfill('0') << std::setw(3) << milliseconds
              << "] ";
}

void Server::printLog(const SavedClient& client, const CallSim::Message& message) {
    printTimestamp(message);
    std::cout << "Client " << client._name << " is " << getStateName(client._state) << '\n';
}

void Server::printLog(const SavedClient& client) {
    printTimestamp(std::chrono::system_clock::now());
    std::cout << "Client " << client._name << " is " << getStateName(client._state) << '\n';
}

}  // namespace server

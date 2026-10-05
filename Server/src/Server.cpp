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

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::talking, CallSim::MessageType::Text),
        std::make_pair(common::ClientStates::talking, [this](SavedClient& client, const CallSim::Message& message) {
            resendMessage(client, message);
        }));

    _transitionTable.emplace(
        std::make_pair(common::ClientStates::talking, CallSim::MessageType::End),
        std::make_pair(common::ClientStates::registered, [this](SavedClient& client, const CallSim::Message& message) {
            handleFinishCall(client, message);
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

    handler(client, message);

    client._state = nextState;
    printLog(client, message);
}
void Server::handleClientDisconnected(const unsigned int clientId) {
    const auto clientOptional = findClient(clientId);

    if (!clientOptional.has_value()) {
        printTimestamp(std::chrono::system_clock::now());
        std::cerr << "Client with ID = " << clientId << " not found";
        return;
    }

    SavedClient& client = clientOptional->get();

    printTimestamp(std::chrono::system_clock::now());
    std::cout << "Client with ID = " << clientId << " and name " << client._name <<" disconnected\n";

    _clients.erase(clientId);
    _registeredClientNames.erase(client._name);
}

void Server::checkAddressee(SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    if (addresseeName == message.from_user()) {
        _rwWrapper.putTaskInContext(
            client,
            createMessage(CallSim::MessageType::Rejected, "You cannot call yourself\n"),
            [this](SavedClient& client, const CallSim::Message& responseMsg) { handleMessage(client, responseMsg); });
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

    if (addressee._state == common::ClientStates::registered) {
        const auto responseMsg = createMessage(CallSim::MessageType::Answering, "", client._name);

        _rwWrapper.putTaskInContext(
            addressee, responseMsg, [this](SavedClient& addressee, const CallSim::Message& responseMsg) {
                handleMessage(addressee, responseMsg);
            });
    } else {
        auto responseMsg = createMessage(CallSim::MessageType::Rejected, "Addressee is not available in the moment\n");
        _rwWrapper.putTaskInContext(
            client, responseMsg, [this](SavedClient& client, const CallSim::Message& responseMsg) {
                handleMessage(client, responseMsg);
            });
    }
}

void Server::registerClient(SavedClient& client, const CallSim::Message& message) {
    const std::string& ClientNameFromMessage = message.from_user();

    if (_registeredClientNames.find(ClientNameFromMessage) != _registeredClientNames.end()) {
        _rwWrapper.asyncWrite(
            client._socket,
            createMessage(CallSim::MessageType::Rejected,
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
        checkAddressee(client, message);
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
        addressee,
        createMessage(CallSim::Rejected, "The call was rejected"),
        [this](SavedClient& addressee, const CallSim::Message& responseMsg) { handleMessage(addressee, responseMsg); });

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
    _rwWrapper.putTaskInContext(
        addressee,
        createMessage(CallSim::Accepted, "", client._name),
        [this](SavedClient& addressee, const CallSim::Message& responseMsg) { handleMessage(addressee, responseMsg); });

    _rwWrapper.asyncWrite(
        client._socket, createMessage(CallSim::MessageType::Accepted, "", addresseeName), client._clientId);
}

void Server::handleFinishCall(const SavedClient& client, const CallSim::Message& message) {
    if (client._name == message.from_user()) {
        _rwWrapper.asyncWrite(client._socket,
                              createMessage(CallSim::Ended, "The call was finished", message.from_user()),
                              client._clientId);

        const std::string& addresseeName = message.to_user();
        const auto addresseeOptional = findClient(addresseeName);

        if (!addresseeOptional) {
            printTimestamp(std::chrono::system_clock::now());
            std::cout << "Client with name " << addresseeName << " is not found\n";
            return;
        }

        SavedClient& addressee = addresseeOptional->get();
        _rwWrapper.putTaskInContext(
            addressee, message, [this](SavedClient& addressee, const CallSim::Message& responseMsg) {
                handleMessage(addressee, responseMsg);
            });
    }

    if (client._name == message.to_user()) {
        _rwWrapper.asyncWrite(
            client._socket,
            createMessage(CallSim::Ended, "The call was finished by another user", message.from_user()),
            client._clientId);
    }
}

void Server::resendMessage(const SavedClient& client, const CallSim::Message& message) {
    const std::string& addresseeName = message.to_user();
    const auto addresseeOptional = findClient(addresseeName);

    if (!addresseeOptional) {
        printTimestamp(std::chrono::system_clock::now());
        std::cout << "Client with name " << addresseeName << " is not found\n";
        return;
    }

    const SavedClient& addressee = addresseeOptional->get();
    _rwWrapper.asyncWrite(
        addressee._socket, createMessage(CallSim::Text, message.payload(), client._name), addressee._clientId);
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

void Server::writeMessage(const SavedClient& client, const CallSim::Message& message) {
    _rwWrapper.asyncWrite(client._socket, message, client._clientId);
}

CallSim::Message Server::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload,
                                       const std::string& fromUser) {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_payload(msgPayload);
    message.set_from_user(fromUser);

    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    message.set_timestamp(timestamp);

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

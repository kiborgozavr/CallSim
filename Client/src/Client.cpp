#include "Client.hpp"
#include "Connected.hpp"
#include "Message.pb.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace client {
Client::Client(const unsigned int port, std::string clientName) :
        _rwWrapper(*this, port),
        _clientName(std::move(clientName)),
        _state(std::make_unique<Connected>()) {
    _state->enter();
}

Client::~Client() { stop(); }

void Client::start() {
    _rwWrapper.start();

    const CallSim::Message message = createRegisterMessage(_clientName);
    _rwWrapper.asyncWrite(message);

    readFromConsole();
    stop();
}

void Client::stop() { _rwWrapper.stop(); }

void Client::write(const CallSim::Message& message) { _rwWrapper.asyncWrite(message); }

CallSim::Message Client::createMessage(const CallSim::MessageType msgType, const std::string& toUser = "",
                                       const std::string& msgPayload = "") {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_from_user(_clientName);
    message.set_to_user(toUser);
    message.set_payload(msgPayload);
    setTimestamp(message);

    return message;
}

CallSim::Message Client::createRegisterMessage(std::string fromUser) {
    CallSim::Message message = createMessage(CallSim::Register);
    message.set_from_user(fromUser);
    return message;
}

void Client::setTimestamp(CallSim::Message& message) {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    message.set_timestamp(timestamp);
}

void Client::readFromConsole() {
    std::string line;

    bool continueReading = true;

    while (continueReading) {
        std::getline(std::cin, line);

        handleConsoleInput(line, continueReading);
    }
}

std::vector<std::string> Client::parseInput(const std::string& line) {
    std::istringstream iss(line);

    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Client::handleConsoleInput(const std::string& line, bool& continueReadingOut) {
    if (_state->getState() == common::ClientStates::REGISTERED) {
        const auto tokens = parseInput(line);

        if (tokens.empty() || tokens.size() > 2) {
            std::cerr << "Invalid input. Please try again.\n";
        }

        const std::string& command = tokens[0];

        if (command == "exit") {
            continueReadingOut = false;

        } else if (command == "call") {
            const std::string& contact = tokens[1];
            _contactName = contact;
            const CallSim::Message message = createMessage(CallSim::MessageType::Call, contact);
            write(message);

        } else {
            std::cerr << "Unknown command. Please try again.\n";
        }

    } else if (_state->getState() == common::ClientStates::CALLING) {
        const CallSim::Message message = createMessage(CallSim::Text, _contactName, line);
        write(message);

    } else {
        std::cerr << "You are not registered on the server. Please, restart the application.\n";
    }
}

void Client::nextState(const CallSim::MessageType msgType) {
    if (auto newState = _state->transition(msgType); newState) {
        _state = std::move(newState);
        _state->enter();
        return;
    }
    std::cerr << "Undefined transition. Client stays in state: " << getStateName(_state->getState()) << '\n';
}

void Client::handleMessage(const CallSim::Message& message) {
    std::cout << "in handle: " << message.type() << ": " << message.payload() << "\n";
    switch (message.type()) {
        case CallSim::MessageType::Registered:
            nextState(CallSim::MessageType::Registered);
            break;
        case CallSim::MessageType::Text:
            printMessage(message);
            break;
        case CallSim::MessageType::Calling:
            nextState(CallSim::MessageType::Calling);
            break;
    }
}

void Client::printMessage(const CallSim::Message& message) {
    std::cout << message.from_user() << ": ";
    std::cout << message.payload() << "\n";
}

}  // namespace client

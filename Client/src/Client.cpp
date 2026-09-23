#include "Client.hpp"
#include "Connected.hpp"
#include "Message.pb.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace client {
Client::Client(const unsigned int port, std::string clientName) :
        _rwWrapper(*this, port),
        _clientName(std::move(clientName)),
        _state(std::make_unique<Connected>()) {
    _state->enter();
}

void Client::start() {
    _rwWrapper.start();

    const CallSim::Message message = createMessage(CallSim::MessageType::Register);
    _rwWrapper.asyncWrite(message);

    readFromConsole();
    stop();
}

void Client::stop() { _rwWrapper.stop(); }

void Client::write(const CallSim::Message& message) { _rwWrapper.asyncWrite(message); }

CallSim::Message Client::createMessage(const CallSim::MessageType msgType, const std::string& toUser,
                                       const std::string& msgPayload) {
    CallSim::Message message;

    message.set_type(msgType);
    message.set_from_user(_clientName);
    message.set_to_user(toUser);
    message.set_payload(msgPayload);
    setTimestamp(message);

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
        if (line == "exit") {
            continueReading = false;
        }
        _state->handleConsoleInput(*this, line);
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

void Client::handleMessage(
    const CallSim::Message& message) {  // - I left this method because MessageType::Text won't change the state
    nextState(message.type());
}

std::string& Client::getContactName() { return _contactName; }
void Client::setContactName(const std::string& contactName) { _contactName = contactName; }

}  // namespace client

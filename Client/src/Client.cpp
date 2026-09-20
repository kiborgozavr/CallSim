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

Client::~Client() { stop(); }

void Client::start() {
    _rwWrapper.start();

    const CallSim::Message message = createRegisterMessage(_clientName);
    _rwWrapper.asyncWrite(message);

    readFromConsole();
    stop();
}

void Client::stop() { _rwWrapper.stop(); }

void Client::write(const std::string& msg) {
    const CallSim::Message message = createMessage(CallSim::Text, msg);
    _rwWrapper.asyncWrite(message);
}

CallSim::Message Client::createMessage(const CallSim::MessageType msgType, const std::string& msgPayload = "") {
    CallSim::Message message;

    message.set_type(msgType);
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

    while (std::getline(std::cin, line)) {
        if (line == "exit") {
            break;
        }
        write(line);
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
    switch (message.type()) {
        case CallSim::MessageType::Registered:
            nextState(CallSim::MessageType::Registered);
            break;
        case CallSim::MessageType::Text:
            printMessage(message);
            break;
    }
}

void Client::printMessage(const CallSim::Message& message) { std::cout << message.payload() << "\n"; }

}  // namespace client

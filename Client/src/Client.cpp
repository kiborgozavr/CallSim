#include "../inc/Client.hpp"
#include "Message.pb.h"
#include "stateMachine/Connected.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace client {
Client::Client(const unsigned int port) :
        _rwWrapper(port),
        _clientId("John Doe"),
        _state(std::make_unique<Connected>()) {
    _state->enter();
    std::cout << "Client connected!\n";
}
void Client::start() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "R") {
            nextState(Register);
            break;
        }
        std::cout << "Client isn't registered! Messages exchange is denied!\n";
    }
    startExchangingMessages();
}
void Client::startExchangingMessages() {
    _rwWrapper.asyncRead();
    _rwWrapper.start();

    _consoleThread = std::thread([this] { readFromConsole(); });

    _consoleThread.join();
}

void Client::stop() {
    _rwWrapper.stop();
    nextState(Unregister);
}

void Client::write(const std::string& msg) {
    const CallSim::Message message = createMessage(CallSim::Text, msg);
    _rwWrapper.asyncWrite(message);
}

CallSim::Message Client::createMessage(CallSim::MessageType msgType, std::string msgPayload) {
    CallSim::Message message;
    message.set_type(msgType);

    message.set_payload(msgPayload);

    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    message.set_timestamp(timestamp);

    return message;
}

void Client::readFromConsole() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "U") {
            stop();
            break;
        }
        write(line);
    }
}
void Client::nextState(const Events event) {
    if (auto newState = _state->transition(event); newState) {
        _state = std::move(newState);
        _state->enter();
        return;
    }
    std::cerr << "Undefined transition. Client stays in state: " << _state->getStateName() << '\n';
}
}  // namespace client

#include "Client.hpp"
#include "Connected.hpp"
#include "ConsoleReader.hpp"
#include "Message.pb.h"
#include "RwWrapper.hpp"
#include "StateFactory.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace client {
Client::Client(unsigned int port, std::string clientName) :
        _rwWrapper(std::make_unique<RwWrapper>(*this, port)),
        _consoleReader(std::make_unique<ConsoleReader>()),
        _clientName(std::move(clientName)),
        _state(std::make_unique<Connected>(*this)) {}

Client::Client(std::unique_ptr<IRwWrapper> rwWrapper, std::unique_ptr<IConsoleReader> consoleReader,
               std::string clientName) :
        _rwWrapper(std::move(rwWrapper)),
        _consoleReader(std::move(consoleReader)),
        _clientName(std::move(clientName)),
        _state(std::make_unique<Connected>(*this)) {}

void Client::start() {
    _rwWrapper->start();

    const CallSim::Message message = createMessage(CallSim::MessageType::Register);
    _rwWrapper->asyncWrite(message);

    readFromConsole();
    _rwWrapper->stop();
}

void Client::write(const CallSim::Message& message) { _rwWrapper->asyncWrite(message); }

void Client::acceptCall() {
    const CallSim::Message message = createMessage(CallSim::MessageType::Accept, _contactName);
    write(message);
}

void Client::rejectCall() {
    const CallSim::Message message = createMessage(CallSim::MessageType::Reject, _contactName);
    write(message);
    _clientName = "";
}
void Client::finishCall() {
    const CallSim::Message message = createMessage(CallSim::MessageType::End, _contactName);
    write(message);
    _contactName = "";
}

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
    while (true) {
        std::string line = _consoleReader->readLine();
        if (line == "exit") {
            break;
        }
        _state->handleConsoleInput(line);
    }
}

void Client::nextState(const CallSim::Message& message) {
    if (const auto newState = _state->transition(message); newState != _state->getState()) {
        _state = _stateFactory.getState(*this, newState);
        _state->enter(message);
        return;
    }
    std::cerr << "Undefined transition. Client stays in state: " << getStateName(_state->getState()) << '\n';
}

void Client::handleMessage(const CallSim::Message& message) {
    if (message.type() == CallSim::MessageType::Answering) {
        this->_contactName = message.from_user();
    }
    if (message.type() == CallSim::MessageType::Ended) {
        this->_contactName = "";
    }
    if (message.type() == CallSim::MessageType::Text) {
        std::cout << message.from_user() << ": " << message.payload() << '\n';
        return;
    }

    nextState(message);
}

std::string& Client::getContactName() { return _contactName; }
void Client::setContactName(const std::string& contactName) { _contactName = contactName; }
common::ClientStates Client::getState() const { return _state->getState(); }

}  // namespace client

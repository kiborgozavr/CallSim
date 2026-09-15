#include "../inc/Client.hpp"
#include "Message.pb.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace client {
Client::Client(const unsigned int port) :
        _rwWrapper(port),
        _clientId("John Doe") {
    std::cout << "Client connected!\n";
}
void Client::start() {
    _rwWrapper.asyncRead();
    _rwWrapper.start();

    _consoleThread = std::thread([this] { readFromConsole(); });

    _consoleThread.join();
}
void Client::stop() { _rwWrapper.stop(); }

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
        if (line == "quit") {
            stop();
            break;
        }
        write(line);
    }
}
}  // namespace client

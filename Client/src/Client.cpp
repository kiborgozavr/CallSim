#include "../inc/Client.hpp"
#include "Message.pb.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <iostream>
#include <thread>

namespace client {
Client::Client(const unsigned int port) :
        _rwWrapper(port),
        _clientId(generateUuid()) {
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
    CallSim::Message message = createMessage(CallSim::Text, this->_clientId, msg);
    _rwWrapper.asyncWrite(message);
}

CallSim::Message Client::createMessage(const CallSim::MessageType messageType, std::string fromUser,
                                       std::string payload) {
    CallSim::Message message;
    message.set_type(messageType);

    message.set_from_user(fromUser);

    message.set_payload(payload);

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

std::string Client::generateUuid() {
    boost::uuids::random_generator generator;
    const boost::uuids::uuid uuid = generator();

    return boost::uuids::to_string(uuid);
}
}  // namespace client

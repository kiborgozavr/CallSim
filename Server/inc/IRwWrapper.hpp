#pragma once

#include "Message.pb.h"

#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <memory>

namespace server {

struct SavedClient;

class IRwWrapper {
public:
    using tcp = boost::asio::ip::tcp;

    virtual ~IRwWrapper() = default;

    virtual void start() = 0;

    virtual void putTaskInContext(
        SavedClient& client,
        const CallSim::Message& message,
        std::function<void(SavedClient&, const CallSim::Message&)> function) = 0;

    virtual void asyncWrite(
        std::shared_ptr<tcp::socket> socket,
        const CallSim::Message& message,
        unsigned int clientId) = 0;
};

}  // namespace server
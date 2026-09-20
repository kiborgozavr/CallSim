#pragma once

#include "ClientStates.hpp"
#include "Message.pb.h"

#include <memory>

namespace client {

class IState {
public:
    virtual ~IState() = default;

    virtual std::unique_ptr<IState> transition(const CallSim::MessageType& msgType) = 0;

    virtual void enter() = 0;

    [[nodiscard]] virtual common::ClientStates getState() const = 0;
};

}  // namespace client

#pragma once

#include "Message.pb.h"

namespace client {

class IRwWrapper {
public:
    virtual ~IRwWrapper() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void asyncWrite(CallSim::Message message) = 0;
    virtual void asyncRead() = 0;
};
}  // namespace client
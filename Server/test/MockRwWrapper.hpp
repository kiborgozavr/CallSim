#pragma once

#include "IRwWrapper.hpp"

#include <gmock/gmock.h>

namespace server {

class MockRwWrapper : public IRwWrapper {
public:
    MOCK_METHOD(void, start, (), (override));

    MOCK_METHOD(void, putTaskInContext,
                (SavedClient & client, const CallSim::Message& message,
                 std::function<void(SavedClient&, const CallSim::Message&)> function),
                (override));

    MOCK_METHOD(void, asyncWrite,
                (std::shared_ptr<tcp::socket> socket, const CallSim::Message& message, unsigned int clientId),
                (override));
};

}  // namespace server
#pragma once

#include "IRwWrapper.hpp"

#include <gmock/gmock.h>

namespace client {

class MockRwWrapper : public IRwWrapper {
public:
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, asyncWrite, (CallSim::Message message), (override));
    MOCK_METHOD(void, asyncRead, (), (override));
};

}  // namespace client
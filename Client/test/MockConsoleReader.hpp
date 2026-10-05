#pragma once

#include "IConsoleReader.hpp"

#include <gmock/gmock.h>

namespace client {

class MockConsoleReader : public IConsoleReader {
public:
    MOCK_METHOD(std::string, readLine, (), (override));
};
}  // namespace client
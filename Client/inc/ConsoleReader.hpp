#pragma once

#include "IConsoleReader.hpp"

namespace client {

class ConsoleReader : public IConsoleReader {
public:
    std::string readLine() override;
};
}  // namespace client

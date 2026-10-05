#pragma once

#include <string>
namespace client {
class IConsoleReader {
public:
    virtual ~IConsoleReader() = default;
    virtual std::string readLine() = 0;
};

class ConsoleReader : public IConsoleReader {
public:
    std::string readLine() override;
};
}  // namespace client
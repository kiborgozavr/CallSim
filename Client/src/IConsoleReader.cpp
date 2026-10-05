#include "../inc/IConsoleReader.hpp"

#include <iostream>

namespace client {

std::string ConsoleReader::readLine() {
    std::string line;

    std::getline(std::cin, line);
    return line;
}
}  // namespace client

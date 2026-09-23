#include "IState.hpp"

#include <sstream>

std::vector<std::string> client::IState::parseInput(const std::string& line) {
    std::istringstream iss(line);

    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
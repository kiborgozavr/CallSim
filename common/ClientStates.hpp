#pragma once

#include <string_view>

namespace common {
enum class ClientStates {
    connected,
    registered,
    calling,
    answering,
    talking,
};

constexpr std::string_view getStateName(const ClientStates state) {
    switch (state) {
        case ClientStates::connected:
            return "Connected";
        case ClientStates::registered:
            return "Registered";
        case ClientStates::calling:
            return "Calling";
        case ClientStates::answering:
            return "Answering";
        case ClientStates::talking:
            return "Talking";
    }

    return "Unknown";
}
}  // namespace common

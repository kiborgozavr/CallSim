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
            return "CONNECTED";
        case ClientStates::registered:
            return "REGISTERED";
        case ClientStates::calling:
            return "CALLING";
        case ClientStates::answering:
            return "ANSWERING";
        case ClientStates::talking:
            return "TALKING";
    }

    return "Unknown";
}
}  // namespace common

#pragma once

#include <string_view>

namespace common {
enum class ClientStates {
    connected,
    registered,
    calling,
};

constexpr std::string_view getStateName(const ClientStates state) {
    switch (state) {
        case ClientStates::connected:
            return "Connected";
        case ClientStates::registered:
            return "Registered";
        case ClientStates::calling:
            return "Calling";
    }

    return "Unknown";
}
}  // namespace common

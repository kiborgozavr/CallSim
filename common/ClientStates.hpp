#pragma once

#include <string_view>

namespace common {
enum class ClientStates {
    CONNECTED,
    REGISTERED,
};

constexpr std::string_view getStateName(const ClientStates state) {
    switch (state) {
        case ClientStates::CONNECTED:
            return "Connected";
        case ClientStates::REGISTERED:
            return "Registered";
    }

    return "Unknown";
}
}  // namespace common

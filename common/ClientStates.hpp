#pragma once

#include <string_view>

namespace common {
enum class ClientStates {
    connected,
    registered,
};

constexpr std::string_view getStateName(const ClientStates state) {
    switch (state) {
        case ClientStates::connected:
            return "Connected";
        case ClientStates::registered:
            return "Registered";
    }

    return "Unknown";
}
}  // namespace common

#pragma once

#include "../Events.hpp"

#include <memory>

namespace client {

class IState {
public:
    virtual ~IState() = default;

    virtual std::unique_ptr<IState> transition(Events) = 0;

    virtual std::string get_name() const = 0;

    virtual void enter() = 0;
};

}  // namespace client

#include "Answering.hpp"
#include "Calling.hpp"
#include "Connected.hpp"
#include "Registered.hpp"
#include "StateFactory.hpp"
#include "Talking.hpp"

namespace client {

std::unique_ptr<IState> StateFactory::getState(Client& client, const common::ClientStates clientState) {
    switch (clientState) {
        case common::ClientStates::connected:
            return std::make_unique<Connected>(client);
        case common::ClientStates::registered:
            return std::make_unique<Registered>(client);
        case common::ClientStates::calling:
            return std::make_unique<Calling>(client);
        case common::ClientStates::answering:
            return std::make_unique<Answering>(client);
        case common::ClientStates::talking:
            return std::make_unique<Talking>(client);
    }
    return nullptr;
}

}  // namespace client
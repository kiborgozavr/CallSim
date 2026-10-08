#include "Client.hpp"
#include "Connected.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using StrictMockRwWrapper = testing::StrictMock<client::MockRwWrapper>;
using StrictMockConsoleReader = testing::StrictMock<client::MockConsoleReader>;

class ConnectedTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();
        auto mockConsoleReader = std::make_unique<StrictMockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
        _state = std::make_unique<client::Connected>(*_client);
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
    std::unique_ptr<client::Connected> _state;
};

TEST_F(ConnectedTest, GetStateReturnsConnected) {
    const auto result = _state->getState();
    EXPECT_EQ(result, common::ClientStates::connected);
}

TEST_F(ConnectedTest, RejectedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Registered);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(ConnectedTest, OtherMessageKeepsConnected) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Call);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::connected);
}

TEST_F(ConnectedTest, AnyInputDoesNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("some text");
}
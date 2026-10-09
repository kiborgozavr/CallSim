#include "Calling.hpp"
#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using StrictMockRwWrapper = testing::StrictMock<client::MockRwWrapper>;
using StrictMockConsoleReader = testing::StrictMock<client::MockConsoleReader>;

class CallingTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();
        auto mockConsoleReader = std::make_unique<StrictMockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
        auto state = std::make_unique<client::Calling>(*_client);
        _state = std::make_unique<client::Calling>(*_client);
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
    std::unique_ptr<client::Calling> _state;
};

TEST_F(CallingTest, GetStateReturnsCalling) {
    const auto result = _state->getState();
    EXPECT_EQ(result, common::ClientStates::calling);
}

TEST_F(CallingTest, RejectedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Rejected);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(CallingTest, AcceptedMessageReturnsTalking) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Accepted);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::talking);
}

TEST_F(CallingTest, OtherMessageKeepsCalling) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::calling);
}

TEST_F(CallingTest, AnyInputDoesNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("some text");
}
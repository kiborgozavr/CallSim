#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"
#include "Talking.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using StrictMockRwWrapper = testing::StrictMock<client::MockRwWrapper>;
using StrictMockConsoleReader = testing::StrictMock<client::MockConsoleReader>;

class TalkingTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();
        auto mockConsoleReader = std::make_unique<StrictMockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
        _state = std::make_unique<client::Talking>(*_client);
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
    std::unique_ptr<client::Talking> _state;
};

TEST_F(TalkingTest, GetStateReturnsTalking) {
    const auto result = _state->getState();
    EXPECT_EQ(result, common::ClientStates::talking);
}

TEST_F(TalkingTest, EndedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Ended);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(TalkingTest, OtherMessageKeepsTalking) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::talking);
}

TEST_F(TalkingTest, EmptyInputDoesNothing) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);
    _state->handleConsoleInput("");
}

TEST_F(TalkingTest, TextInputSendsTextMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::Truly([](const CallSim::Message& message) {
        return message.type() == CallSim::Text && message.payload() == "Hello";
    })));

    _state->handleConsoleInput("Hello");
}

TEST_F(TalkingTest, EndInputSendsTextMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::Truly([](const CallSim::Message& message) {
        return message.type() == CallSim::End;
    })));

    _state->handleConsoleInput("end");
}
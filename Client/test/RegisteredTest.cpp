#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"
#include "Registered.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::Truly;

using StrictMockRwWrapper = testing::StrictMock<client::MockRwWrapper>;
using StrictMockConsoleReader = testing::StrictMock<client::MockConsoleReader>;

class RegisteredTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();
        auto mockConsoleReader = std::make_unique<StrictMockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
        _state = std::make_unique<client::Registered>(*_client);
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
    std::unique_ptr<client::Registered> _state;
};

TEST_F(RegisteredTest, GetStateReturnsRegistered) {
    const auto result = _state->getState();
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(RegisteredTest, CallingMessageReturnsCalling) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Calling);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::calling);
}

TEST_F(RegisteredTest, AnsweringMessageReturnsAnswering) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Answering);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::answering);
}

TEST_F(RegisteredTest, OtherMessageKeepsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);

    const auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(RegisteredTest, CallCommandSetsContactAndSendsMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(Truly([](const CallSim::Message& message) {
        return message.type() == CallSim::MessageType::Call && message.to_user() == "Bob";
    })));

    _state->handleConsoleInput("call Bob");

    EXPECT_EQ(_client->getContactName(), "Bob");
}

TEST_F(RegisteredTest, InvalidArgumentsDoNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("call");
    _state->handleConsoleInput("call Bob Extra");
}

TEST_F(RegisteredTest, UnknownCommandDoesNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("hello Bob");
}

TEST_F(RegisteredTest, EmptyInputDoesNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("");
}
#include "Answering.hpp"
#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Truly;

using StrictMockRwWrapper = testing::StrictMock<client::MockRwWrapper>;
using StrictMockConsoleReader = testing::StrictMock<client::MockConsoleReader>;

class AnsweringTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();
        auto mockConsoleReader = std::make_unique<StrictMockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
        _state = std::make_unique<client::Answering>(*_client);
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
    std::unique_ptr<client::Answering> _state;
};

TEST_F(AnsweringTest, GetStateReturnsAnswering) {
    const auto result = _state->getState();
    EXPECT_EQ(result, common::ClientStates::answering);
}

TEST_F(AnsweringTest, RejectedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Rejected);

    auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::registered);
}

TEST_F(AnsweringTest, AcceptedMessageReturnsTalking) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Accepted);

    auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::talking);
}

TEST_F(AnsweringTest, OtherMessageKeepsAnswering) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);

    auto result = _state->transition(message);
    EXPECT_EQ(result, common::ClientStates::answering);
}

TEST_F(AnsweringTest, YesCommandSendsAcceptMessage) {
    _client->setContactName("Bob");

    EXPECT_CALL(*_mockRwWrapper, asyncWrite(Truly([](const CallSim::Message& message) {
        return message.type() == CallSim::MessageType::Accept && message.to_user() == "Bob";
    })));

    _state->handleConsoleInput("yes");
}

TEST_F(AnsweringTest, NoCommandSendsRejectMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(Truly([](const CallSim::Message& message) {
        return message.type() == CallSim::MessageType::Reject;
    })));

    _state->handleConsoleInput("no");
}

TEST_F(AnsweringTest, UnknownCommandDoesNotSendMessage) {
    EXPECT_CALL(*_mockRwWrapper, asyncWrite(testing::_)).Times(0);

    _state->handleConsoleInput("some");
}
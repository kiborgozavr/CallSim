#include "Answering.hpp"
#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

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
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
};

TEST_F(AnsweringTest, GetStateReturnsAnswering) {
    const auto state = std::make_unique<client::Answering>(*_client);

    EXPECT_EQ(state->getState(), common::ClientStates::answering);
}

TEST_F(AnsweringTest, RejectedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Rejected);
    const auto state = std::make_unique<client::Answering>(*_client);

    EXPECT_EQ(state->transition(message), common::ClientStates::registered);
}

TEST_F(AnsweringTest, AcceptedMessageReturnsTalking) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Accepted);
    const auto state = std::make_unique<client::Answering>(*_client);

    EXPECT_EQ(state->transition(message), common::ClientStates::talking);
}

TEST_F(AnsweringTest, OtherMessageKeepsAnswering) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);
    const auto state = std::make_unique<client::Answering>(*_client);

    EXPECT_EQ(state->transition(message), common::ClientStates::answering);
}

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
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
};

TEST_F(TalkingTest, GetStateReturnsTalking) {
    const auto state = std::make_unique<client::Talking>(*_client);

    EXPECT_EQ(state->getState(), common::ClientStates::talking);
}

TEST_F(TalkingTest, EndedMessageReturnsRegistered) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Ended);
    const auto state = std::make_unique<client::Talking>(*_client);

    EXPECT_EQ(state->transition(message), common::ClientStates::registered);
}

TEST_F(TalkingTest, OtherMessageKeepsTalking) {
    CallSim::Message message;
    message.set_type(CallSim::MessageType::Register);
    const auto state = std::make_unique<client::Talking>(*_client);

    EXPECT_EQ(state->transition(message), common::ClientStates::talking);
}

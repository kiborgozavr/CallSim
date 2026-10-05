#include "Client.hpp"
#include "Message.pb.h"
#include "MockConsoleReader.hpp"
#include "MockRwWrapper.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Truly;

class ClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<client::MockRwWrapper>();
        auto mockConsoleReader = std::make_unique<client::MockConsoleReader>();

        _mockRwWrapper = mockRwWrapper.get();
        _mockConsoleReader = mockConsoleReader.get();

        _client = std::make_unique<client::Client>(std::move(mockRwWrapper), std::move(mockConsoleReader), "Alice");
    }

    client::MockRwWrapper* _mockRwWrapper = nullptr;
    client::MockConsoleReader* _mockConsoleReader = nullptr;
    std::unique_ptr<client::Client> _client;
};

TEST_F(ClientTest, RegisteredClientCanTransitionToCalling) {
    using ::testing::InSequence;
    using ::testing::Invoke;
    using ::testing::Return;
    using ::testing::Truly;

    InSequence sequence;

    EXPECT_CALL(*_mockRwWrapper, start());

    EXPECT_CALL(*_mockRwWrapper, asyncWrite(Truly([](CallSim::Message message) {
        return message.type() == CallSim::MessageType::Register;
    }))).WillOnce(Invoke([this](const CallSim::Message&) {
        CallSim::Message registeredMessage;
        registeredMessage.set_type(CallSim::MessageType::Registered);

        _client->handleMessage(registeredMessage);
    }));

    EXPECT_CALL(*_mockConsoleReader, readLine()).WillOnce(Return("call Bob"));

    EXPECT_CALL(*_mockRwWrapper, asyncWrite(Truly([](CallSim::Message message) {
        return message.type() == CallSim::MessageType::Call && message.to_user() == "Bob";
    }))).WillOnce(Invoke([this](const CallSim::Message&) {
        CallSim::Message callingMessage;
        callingMessage.set_type(CallSim::MessageType::Calling);

        _client->handleMessage(callingMessage);
    }));

    EXPECT_CALL(*_mockConsoleReader, readLine()).WillOnce(Return("exit"));

    EXPECT_CALL(*_mockRwWrapper, stop());

    _client->start();

    EXPECT_EQ(_client->getState(), common::ClientStates::calling);
}
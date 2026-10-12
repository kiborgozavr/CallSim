#include "Message.pb.h"
#include "MockRwWrapper.hpp"
#include "Server.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using testing::_;
using testing::Invoke;
using testing::StrictMock;
using testing::Truly;

using tcp = boost::asio::ip::tcp;
using StrictMockRwWrapper = StrictMock<server::MockRwWrapper>;

class ServerTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();

        _mockRwWrapper = mockRwWrapper.get();
        _server = std::make_unique<server::Server>(std::move(mockRwWrapper));

        EXPECT_CALL(*_mockRwWrapper, start()).Times(1);
        _server->start();
    }

    void createRegisteredClients() {
        _aliceId = getRegisteredClient("Alice", _aliceSocket);
        _bobId = getRegisteredClient("Bob", _bobSocket);
    }

    void expectPutTaskInContext() {
        ON_CALL(*_mockRwWrapper, putTaskInContext(_, _, _))
            .WillByDefault(Invoke([](server::SavedClient& client,
                                     const CallSim::Message& message,
                                     std::function<void(server::SavedClient&, const CallSim::Message&)>
                                         function) { function(client, message); }));
    }

    void startCall() {
        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_aliceSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Calling;
                               }),
                               _aliceId))
            .Times(1);

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_bobSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Answering &&
                                          message.from_user() == "Alice";
                               }),
                               _bobId))
            .Times(1);

        _server->handleMessage(makeMessage(CallSim::MessageType::Call, "Alice", "Bob"), _aliceId);
    }

    void acceptCall() {
        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_aliceSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Accepted &&
                                          message.from_user() == "Bob";
                               }),
                               _aliceId))
            .Times(1);

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_bobSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Accepted &&
                                          message.from_user() == "Alice";
                               }),
                               _bobId))
            .Times(1);

        _server->handleMessage(makeMessage(CallSim::MessageType::Accept, "Bob", "Alice"), _bobId);
    }

    void rejectCall() {
        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_aliceSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Rejected;
                               }),
                               _aliceId))
            .Times(1);

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(_bobSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Rejected;
                               }),
                               _bobId))
            .Times(1);
    }

    unsigned int getConnectedClient(std::shared_ptr<tcp::socket>& outSocket) {
        auto socket = std::make_shared<tcp::socket>(_ioContext);
        outSocket = socket;

        return _server->saveClient(socket);
    }

    unsigned int getRegisteredClient(const std::string& name, std::shared_ptr<tcp::socket>& outSocket) {
        const unsigned int clientId = getConnectedClient(outSocket);

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(outSocket,
                               Truly([](const CallSim::Message& message) {
                                   return message.type() == CallSim::MessageType::Registered;
                               }),
                               clientId))
            .Times(1);

        _server->handleMessage(makeMessage(CallSim::MessageType::Register, name), clientId);

        return clientId;
    }

    CallSim::Message makeMessage(const CallSim::MessageType type, const std::string& from, const std::string& to = "",
                                 const std::string& payload = "") {
        CallSim::Message message;

        message.set_type(type);
        message.set_from_user(from);
        message.set_to_user(to);
        message.set_payload(payload);

        return message;
    }

    server::MockRwWrapper* _mockRwWrapper = nullptr;
    std::unique_ptr<server::Server> _server;
    boost::asio::io_context _ioContext;

    std::shared_ptr<tcp::socket> _aliceSocket;
    std::shared_ptr<tcp::socket> _bobSocket;

    unsigned int _aliceId = 0;
    unsigned int _bobId = 0;
};

TEST_F(ServerTest, ClientConnects) {
    std::shared_ptr<tcp::socket> socket;

    const unsigned int clientId = getConnectedClient(socket);

    EXPECT_EQ(clientId, 0);
}

TEST_F(ServerTest, ClientRegisters) {
    std::shared_ptr<tcp::socket> socket;

    const unsigned int clientId = getRegisteredClient("Alice", socket);

    EXPECT_EQ(clientId, 0);
}

TEST_F(ServerTest, ClientEnterStateCalling) {
    std::shared_ptr<tcp::socket> socket;
    const unsigned int clientId = getRegisteredClient("Alice", socket);

    expectPutTaskInContext();

    EXPECT_CALL(*_mockRwWrapper,
                asyncWrite(_,
                           Truly([](const CallSim::Message& message) {
                               return message.type() == CallSim::MessageType::Calling;
                           }),
                           clientId))
        .Times(1);

    EXPECT_CALL(*_mockRwWrapper, putTaskInContext(_, _, _)).Times(1);

    _server->handleMessage(makeMessage(CallSim::MessageType::Call, "Alice", "Bob"), clientId);
}

TEST_F(ServerTest, ClientEnterStateAnswering) {
    createRegisteredClients();
    expectPutTaskInContext();

    startCall();
}

TEST_F(ServerTest, ClientRejectsCall) {
    createRegisteredClients();
    expectPutTaskInContext();

    startCall();
    rejectCall();

    _server->handleMessage(makeMessage(CallSim::MessageType::Reject, "Bob", "Alice"), _bobId);
}

TEST_F(ServerTest, ClientAcceptsCallAndSendsMessage) {
    createRegisteredClients();
    expectPutTaskInContext();

    startCall();
    acceptCall();

    const std::string text = "Hello, Bob!";

    EXPECT_CALL(*_mockRwWrapper,
                asyncWrite(_bobSocket,
                           Truly([&text](const CallSim::Message& message) {
                               return message.type() == CallSim::MessageType::Text && message.payload() == text &&
                                      message.from_user() == "Alice";
                           }),
                           _bobId))
        .Times(1);

    _server->handleMessage(makeMessage(CallSim::MessageType::Text, "Alice", "Bob", text), _aliceId);
}

TEST_F(ServerTest, ClientFinishesCall) {
    createRegisteredClients();
    expectPutTaskInContext();

    startCall();
    acceptCall();

    EXPECT_CALL(*_mockRwWrapper,
                asyncWrite(_aliceSocket,
                           Truly([](const CallSim::Message& message) {
                               return message.type() == CallSim::MessageType::Ended &&
                                      message.payload() == "The call was finished" && message.from_user() == "Alice";
                           }),
                           _aliceId))
        .Times(1);

    EXPECT_CALL(*_mockRwWrapper,
                asyncWrite(_bobSocket,
                           Truly([](const CallSim::Message& message) {
                               return message.type() == CallSim::MessageType::Ended &&
                                      message.payload() == "The call was finished by another user" &&
                                      message.from_user() == "Alice";
                           }),
                           _bobId))
        .Times(1);

    _server->handleMessage(makeMessage(CallSim::MessageType::End, "Alice", "Bob"), _aliceId);
}
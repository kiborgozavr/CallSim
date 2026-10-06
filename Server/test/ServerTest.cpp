#include "Message.pb.h"
#include "MockRwWrapper.hpp"
#include "Server.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using testing::InSequence;
using testing::Invoke;
using testing::Return;
using testing::Truly;

using tcp = boost::asio::ip::tcp;
using StrictMockRwWrapper = testing::StrictMock<server::MockRwWrapper>;

class ServerTest : public testing::Test {
protected:
    void SetUp() override {
        auto mockRwWrapper = std::make_unique<StrictMockRwWrapper>();

        _mockRwWrapper = mockRwWrapper.get();

        _server = std::make_unique<server::Server>(std::move(mockRwWrapper));
    }

    void startServer() {
        EXPECT_CALL(*_mockRwWrapper, start()).Times(1);
        _server->start();
    }

    unsigned int connectClient(std::shared_ptr<tcp::socket>& outSocket) {
        auto socket = std::make_shared<tcp::socket>(_ioContext);
        outSocket = socket;
        return _server->saveClient(socket);
    }

    unsigned int connectAndRegisterClient(const std::string& name,
                                          std::shared_ptr<tcp::socket>& outSocket) {
        const unsigned int clientId = connectClient(outSocket);

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(outSocket,
                               Truly([](const CallSim::Message& m) {
                                   return m.type() == CallSim::MessageType::Registered;
                               }),
                               clientId))
            .Times(1);

        _server->handleMessage(makeMessage(CallSim::MessageType::Register, name), clientId);
        return clientId;
    }

    CallSim::Message makeMessage(const CallSim::MessageType type,
                             const std::string& from,
                             const std::string& to = "",
                             const std::string& payload = "") {
        CallSim::Message msg;
        msg.set_type(type);
        msg.set_from_user(from);
        msg.set_to_user(to);
        msg.set_payload(payload);
        return msg;
    }

    server::MockRwWrapper* _mockRwWrapper = nullptr;
    std::unique_ptr<server::Server> _server;
    boost::asio::io_context _ioContext;
};

TEST_F(ServerTest, ClientJoinsAndSendsCall) {
    startServer();

    std::shared_ptr<tcp::socket> socket;
    const unsigned int clientId = connectAndRegisterClient("Alice", socket);

    auto callMsg = makeMessage(CallSim::MessageType::Call, "Alice", "Bob");

    {
        InSequence seq;

        EXPECT_CALL(*_mockRwWrapper,
                    asyncWrite(socket,
                               Truly([](const CallSim::Message& m) {
                                   return m.type() == CallSim::MessageType::Calling;
                               }),
                               clientId))
            .Times(1);

        EXPECT_CALL(*_mockRwWrapper, putTaskInContext(::testing::_, ::testing::_, ::testing::_)).Times(1);
    }

    _server->handleMessage(callMsg, clientId);
}
#include "ClientRwWrapper.h"

#include <iostream>
#include <thread>

ClientRwWrapper::ClientRwWrapper(const unsigned int port) :
        socket(io_context),
        resolver(io_context),
        work_guard(boost::asio::make_work_guard(io_context)) {
    const auto endpoint = resolver.resolve("localhost", std::to_string(port));
    boost::asio::connect(socket, endpoint);
}
void ClientRwWrapper::start() {
    io_thread_1 = std::thread([this] { io_context.run(); });
    io_thread_2 = std::thread([this] { io_context.run(); });
}
void ClientRwWrapper::stop() {
    socket.close();

    work_guard.reset();

    if (io_thread_1.joinable()) {
        io_thread_1.join();
    }
    if (io_thread_2.joinable()) {
        io_thread_2.join();
    }
}
void ClientRwWrapper::async_write(const std::string& message) {
    auto data = std::make_shared<std::string>(message + "\n");

    boost::asio::async_write(
        socket, boost::asio::buffer(*data), [this, data](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                std::cerr << "Write error: " << ec.message() << '\n';
                return;
            }
            async_read();
        });
}
void ClientRwWrapper::async_read() {
    boost::asio::async_read_until(socket, buffer, '\n', [this](const boost::system::error_code& ec, std::size_t) {
        if (ec) {
            std::cerr << "Read error: " << ec.message() << '\n';
            return;
        }

        std::istream stream(&buffer);

        std::string response;
        std::getline(stream, response);

        std::cout << response << '\n';
    });
}
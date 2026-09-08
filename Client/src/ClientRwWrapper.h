#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

class ClientRwWrapper {
public:
    explicit ClientRwWrapper(unsigned int port);

    void start();

    void stop();

    void async_write(const std::string& message);

    void async_read();

private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;

    boost::asio::streambuf buffer;

    std::thread io_thread_1;
    std::thread io_thread_2;
};

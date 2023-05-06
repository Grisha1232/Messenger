//
// Created by Григорий Селезнев on 5/5/23.
//

#include "MNetworking/server/tcp_connection.h"
#include <iostream>

namespace Messenger {
    TCPConnection::TCPConnection(io::ip::tcp::socket&& socket) : socket_(std::move(socket)){
        boost::system::error_code ec;
        std::stringstream name;
        name << socket_.remote_endpoint();

        user_name_ = name.str();
    }

    void TCPConnection::start(messageHandler&& message_handler, errorHandler&& error_handler) {
        message_handler_ = std::move(message_handler);
        error_handler_ = std::move(error_handler);

        asyncRead();
    }


    void TCPConnection::postMessage(const std::string &message) {
        bool isQueueIdle = outgoing_messages_.empty();

        outgoing_messages_.push(message);
        if (isQueueIdle) {
            asyncWrite();
        }
    }

    void TCPConnection::asyncRead() {
        io::async_read_until(socket_, streamBuffer_, "\n", [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred){
          self->onRead(ec, bytesTransferred);
      });
    }

    void TCPConnection::onRead(boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            socket_.close(ec);

            error_handler_();
            return;
        }

        std::stringstream message;
        message << std::istream(&streamBuffer_).rdbuf();
        std::cout << user_name_ << " " << message.str();
        std::stringstream name_socket;
        name_socket << socket_.remote_endpoint();

        if (message.str().substr(0, 9) == "--login--" && name_socket.str() == user_name_) {
            std::string name;
            name = message.str().substr(9, message.str().size() - 2);
            std::erase_if( name,
                      [](auto ch)
                      {
                        return (ch == '\n' ||
                                ch == '\r');
                      });
            if (connected_users_.contains(name)) {
                io::async_write(socket_, io::buffer("reject\n"), [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred) {
                    self->onWrite(ec, bytesTransferred);
                });
            } else {
                user_name_ = name;
                connected_users_.insert(user_name_);
                io::async_write(socket_, io::buffer("ok\n"), [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred) {
                    self->onWrite(ec, bytesTransferred);
                });
                message_handler_("User " + user_name_ + " has been connected\n");
            }
        } else {
            message_handler_(user_name_ + ": " + message.str());
        }

        asyncRead();
    }

    void TCPConnection::asyncWrite() {
        io::async_write(socket_, io::buffer(outgoing_messages_.front()), [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred){
            self->onWrite(ec, bytesTransferred);
        });
    }

    void TCPConnection::onWrite(boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            socket_.close(ec);

            error_handler_();
            return;
        }

        if (!outgoing_messages_.empty()) {
            outgoing_messages_.pop();
        }

        if (!outgoing_messages_.empty()) {
            asyncWrite();
        }

    }
}
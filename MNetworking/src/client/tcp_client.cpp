//
// Created by Григорий Селезнев on 5/6/23.
//

#include <MNetworking/client/tcp_client.h>
#include <iostream>

namespace Messenger {

TCPClient::TCPClient(const std::string &address, int port): socket_(io_context_) {
    io::ip::tcp::resolver resolver {io_context_};
    endpoints_ = resolver.resolve(address, std::to_string(port));
}
void TCPClient::run() {
    io::async_connect(socket_, endpoints_, [this](boost::system::error_code ec, io::ip::tcp::endpoint ep) {
        if (!ec) {
            asyncRead();
        }
    });

    io_context_.run();
}

void TCPClient::stop() {
    boost::system::error_code ec;
    socket_.close(ec);

    if (ec) {

    }
}

void TCPClient::postMessage(const std::string &message) {
    bool isQueueIdle = outgoingMessages_.empty();
    outgoingMessages_.push(message);

    if (isQueueIdle) {
        asyncWrite();
    }
}

void TCPClient::asyncRead() {
    io::async_read_until(socket_, streambuf_, "\n", [this](boost::system::error_code ec, size_t bytesTransferred){
      onRead(ec, bytesTransferred);
    });

}

void TCPClient::onRead(boost::system::error_code ec, size_t bytesTransferred) {
    if (ec) {
        stop();
        return;
    }
    std::stringstream message;
    message << std::istream{&streambuf_}.rdbuf();
    onMessage(message.str());
    asyncRead();
}

void TCPClient::asyncWrite() {
    io::async_write(socket_, io::buffer(outgoingMessages_.front()), [this](boost::system::error_code ec, size_t bytesTransferred){
      onWrite(ec, bytesTransferred);
    });
}

void TCPClient::onWrite(boost::system::error_code ec, size_t bytesTransferred) {
    if (ec) {
      stop();
      return;
    }

    outgoingMessages_.pop();

    if (!outgoingMessages_.empty()) {
        asyncWrite();
    }
}
}
//
// Created by Григорий Селезнев on 5/5/23.
//

#include "MNetworking/server/server.h"

namespace Messenger {
    using boost::asio::ip::tcp;
    TCPServer::TCPServer(IPV ipv, int port) : ipVersion_(ipv), port_(port),
          acceptor_(tcp::acceptor(io_context_, tcp::endpoint(ipVersion_ == IPV::V4 ? tcp::v4() : tcp::v6(), port))) {
    }

    int TCPServer::run() {
        try {
          startAccept();
          io_context_.run();
        } catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
          return -1;
        }
        return 0;
    }

    void TCPServer::startAccept() {
        socket_.emplace(io_context_);

        // asynchronously accept the connection
        acceptor_.async_accept(*socket_, [this](const boost::system::error_code& error) {
            auto connection = TCPConnection::Create(std::move(*socket_));

            if (onJoin) {
                onJoin(connection);
            }

            connections_.insert(connection);

            if (!error) {
              connection->start(
                  [this](const std::string& message) { if (onClientMessage) {onClientMessage(message);} },
                  [&, weak = std::weak_ptr(connection)] {
                      if (auto shared = weak.lock(); shared && connections_.erase(shared)) {
                          if (onLeave) {
                              shared->connected_users_.erase(shared->getUserName());
                              onClientMessage("User " + shared->getUserName() + " has been left\n");
                              onLeave(shared);
                          }
                      }
                  }
              );
            }

            startAccept();
        });
    }


    void TCPServer::brodcast(const std::string& message) {
        for (auto& connection: connections_) {
            connection->postMessage(message);
        }
    }
}

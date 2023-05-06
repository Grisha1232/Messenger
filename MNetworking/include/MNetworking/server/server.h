//
// Created by Григорий Селезнев on 5/5/23.
//

#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "tcp_connection.h"
#include <functional>
#include <optional>
#include <unordered_set>


namespace Messenger {

  namespace io = boost::asio;
  enum  class IPV {
      V4,
      V6
  };

  class TCPServer {
        using onJoinHandler = std::function<void(TCPConnection::pointer)>;
        using onLeaveHandler = std::function<void(TCPConnection::pointer)>;
        using onClientMessageHandler = std::function<void(std::string)>;

    public:
        TCPServer(IPV ipv, int port);

        int run();

        void brodcast(const std::string& message);

        onJoinHandler onJoin;
        onLeaveHandler onLeave;
        onClientMessageHandler onClientMessage;

    private:
        void startAccept();

        IPV ipVersion_;
        int port_;

        io::io_context io_context_;
        io::ip::tcp::acceptor acceptor_;

        std::optional<io::ip::tcp::socket> socket_;

        std::unordered_set<TCPConnection::pointer> connections_ {};
  };

}// namespace Messanger

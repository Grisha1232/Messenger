//
// Created by Григорий Селезнев on 5/5/23.
//

#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <set>

namespace Messenger {
  using boost::asio::ip::tcp;
  namespace io = boost::asio;

  using messageHandler = std::function<void(std::string)>;
  using errorHandler = std::function<void()>;
class TCPConnection: public std::enable_shared_from_this<TCPConnection> {
     public:
        using pointer = std::shared_ptr<TCPConnection>;

        static pointer Create(io::ip::tcp::socket&& socket) {
            return pointer(new TCPConnection(std::move(socket)));
        }

        inline const std::string& getUserName() const {return user_name_;}

        void start(messageHandler&& message_handler, errorHandler&& error_handler);

        void postMessage(const std::string& message);

        tcp::socket& socket() {
          return socket_;
        }

        static inline std::set<std::string> connected_users_ = {};
     private:
        explicit TCPConnection(io::ip::tcp::socket&& socket);

        // wait a new message from client
        void asyncRead();
        void onRead(boost::system::error_code ec, size_t bytesTransferred);

        void asyncWrite();
        void onWrite(boost::system::error_code ec, size_t bytesTransferred);

        tcp::socket socket_;
        std::string user_name_;

        std::queue<std::string> outgoing_messages_;
        io::streambuf streamBuffer_ {65536};

        messageHandler message_handler_;
        errorHandler error_handler_;
    };
}




//
// Created by Григорий Селезнев on 5/6/23.
//

#pragma once
#include <boost/asio.hpp>
#include <queue>

namespace Messenger {

namespace io = boost::asio;
using messageHadler = std::function<void(std::string)>;

class TCPClient {
 public:
      messageHadler onMessage;

      TCPClient(const std::string& address, int port);

      void run();
      void stop();
      void postMessage(const std::string& message);

 private:
      io::io_context io_context_ {};
      io::ip::tcp::socket socket_;

      io::ip::tcp::resolver ::results_type endpoints_;
      io::streambuf streambuf_{65536};

      std::queue<std::string> outgoingMessages_ {};

      void asyncRead();
      void onRead(boost::system::error_code ec, size_t bytesTransferred);

      void asyncWrite();
      void onWrite(boost::system::error_code ec, size_t bytesTransferred);
};
}

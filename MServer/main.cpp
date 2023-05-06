#include "MNetworking/server/server.h"
#include <iostream>

int main() {

  Messenger::TCPServer server(Messenger::IPV::V4, 12345);

  server.onJoin = [](Messenger::TCPConnection::pointer server) {
      std::cout << "User has joined the server: " << server->getUserName() << std::endl;
  };

  server.onLeave = [](Messenger::TCPConnection::pointer server) {
      std::cout << "User has left the server: " << server->getUserName() << std::endl;
  };

  server.onClientMessage = [&server](const std::string& message) {
        server.brodcast(message);
  };
  server.run();

  return 0;
}

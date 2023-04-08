#include <iostream>
#include <boost/asio.hpp>
#include "ChatNetworking/server/TCPServer.h"
using boost::asio::ip::tcp;
int main() {

    Chat::TCPServer server{Chat::IPV::V4, 41480};

    server.OnJoin = [](Chat::TCPConnection::pointer server){
        std::cout << server->getUsername() << " has joined the server\n";
    };

    server.OnLeave = [](Chat::TCPConnection::pointer server){
        std::cout << server->getUsername() << " has left the server\n";
    };

    server.OnClientMessage = [&server](const std::string& message){
        //Parse the messege
        server.Broadcast(message);
    };

    server.run();
    return 0;
}
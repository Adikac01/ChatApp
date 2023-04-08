#include <iostream>
#include <boost/asio.hpp>
#include "ChatNetworking/server/TCPServer.h"
using boost::asio::ip::tcp;
int main() {

    Chat::TCPServer server{Chat::IPV::V4, 1337};

    server.OnJoin = [](Chat::TCPConnection::pointer client){
        std::cout << client->getUsername() << " has joined the server\n";
    };

    server.OnLeave = [](Chat::TCPConnection::pointer client){
        std::cout << client->getUsername() << " has left the server\n";
    };

    server.OnClientMessage = [&server](const std::string& message){
        //Parse the messege
        server.Broadcast(message);
    };

    server.run();
    return 0;
}
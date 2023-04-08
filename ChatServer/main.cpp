#include <iostream>
#include <boost/asio.hpp>
#include "ChatNetworking/server/TCPServer.h"
using boost::asio::ip::tcp;
int main() {

    Chat::TCPServer server{Chat::IPV::V4, 1337};

    server.OnJoin = [](Chat::TCPConnection::pointer client){
        std::cout << client->getUsername() << " has joined the server\n";
    };

    server.OnLeave = [&server](Chat::TCPConnection::pointer client){
        std::cout << client->getUsername() << " has left the server\n";
        std::string msg = client->getUsername() + " has left the chat\n";
        server.Broadcast(msg);
    };

    server.OnClientMessage = [&server](const std::string& message){
        //Parse the messege
        server.Broadcast(message);
    };

    server.OnUsernameSet = [&server](const std::string& message, const Chat::TCPConnection::pointer& ptr){
        server.Broadcast(message,ptr);
    };

    server.run();
    return 0;
}
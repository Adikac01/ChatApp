#include <iostream>
#include <boost/asio.hpp>
#include "ChatNetworking/server/TCPServer.h"

using boost::asio::ip::tcp;

int main() {

    Chat::TCPServer server{Chat::IPV::V4, 1337};

    server.OnJoin = [](const Chat::TCPConnection::pointer& client) {
        std::cout << client->getUsername() << " has joined the server\n";
    };

    server.OnLeave = [&server](const Chat::TCPConnection::pointer& client, const Chat::TCPChatRoom::pointer& chatRoom) {
        std::cout << client->getUsername() << " has left the server\n";
        std::string msg = client->getUsername() + " has left the chat\n";
        server.Broadcast(msg, chatRoom);
    };

    server.OnClientMessage = [&server](const std::string &message, const Chat::TCPChatRoom::pointer & chatRoom) {
        //Parse the messege
        server.Broadcast(message);
    };

    server.OnUsernameSet = [&server](const std::string &message, const Chat::TCPChatRoom::pointer &chatRoom) {
        server.Broadcast(message, chatRoom);
    };


    server.run();
    return 0;
}
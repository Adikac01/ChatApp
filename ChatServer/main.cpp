#include <iostream>
#include <boost/asio.hpp>
#include <ChatNetworking/TCPServer.h>
using boost::asio::ip::tcp;
int main() {

    Chat::TCPServer server{Chat::IPV::V4, 1337};

    server.OnJoin = [](Chat::TCPConnection::pointer server){

    };

    server.OnLeave = [](Chat::TCPConnection::pointer server){

    };


    server.run();
    return 0;
}
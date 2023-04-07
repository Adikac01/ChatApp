#ifndef CHATAPP_TCPSERVER_H
#define CHATAPP_TCPSERVER_H
#include <boost/asio.hpp>
#include <ChatNetworking/TcpConnection.h>

#pragma once

namespace Chat {
    enum class IPV{
        V4,
        V6
    };
    using asio::ip::tcp;
    class TCPServer {
    public:
        //Constructors
        TCPServer(IPV ipv, int port);

        //Functions
        int run();

    private:
        void startAccept();

    private:
        IPV _ipVersion;
        int _port;

        asio::io_context _ioContext;
        tcp::acceptor _acceptor;

        std::vector<TCPConnection::pointer> _connections {};
    };
}

#endif //CHATAPP_TCPSERVER_H

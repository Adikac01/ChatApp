//
// Created by 434578 on 07.04.2023.
//

#include <utility>
#include <iostream>

#include "ChatNetworking/TCPServer.h"
#include "ChatNetworking/TcpConnection.h"

namespace Chat {
    using asio::ip::tcp;
    TCPServer::TCPServer(IPV ipv, int port) : _ipVersion(ipv), _port(port),
        _acceptor(_ioContext, tcp::endpoint(_ipVersion == IPV::V4 ? tcp::v4() : tcp::v6(), _port)) {


    }

    int TCPServer::run(){
        try {
            startAccept();
            _ioContext.run();
        }catch (std::exception& e){
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    void TCPServer::startAccept() {
        auto connection = TCPConnection::Create(_ioContext);
        _connections.push_back(connection);
        //asynchronously accept a connection
        _acceptor.async_accept(connection->getSocket(),
                               [connection, this](const boost::system::error_code& error){
            if(!error){
                connection->Start();
            }else{

            }
            startAccept();
        });
    }

}
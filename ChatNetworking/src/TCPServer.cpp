#include <iostream>
#include "ChatNetworking/TCPServer.h"
#include "ChatNetworking/TcpConnection.h"
#include <memory>


namespace Chat {
    using asio::ip::tcp;
    TCPServer::TCPServer(IPV ipv, int port) : _ipVersion(ipv), _port(port),
        _acceptor(_ioContext, tcp::endpoint(_ipVersion == IPV::V4 ? tcp::v4() : tcp::v6(), _port)){}

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
        _socket.emplace(_ioContext);

        //asynchronously accept a connection
        _acceptor.async_accept(*_socket,[this](const boost::system::error_code& error){
            auto connection = TCPConnection::Create(std::move(*_socket));
            _connections.insert(connection);
            if(!error){
                connection->Start();
            }else{

            }
            startAccept();
        });
    }
}
#include <iostream>
#include "ChatNetworking/server/TCPServer.h"
#include "ChatNetworking/server/TcpConnection.h"
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

    void TCPServer::Broadcast(const std::string &message) {
        for(auto& connection : _connections){
            connection->post(message);
        }
    }

    void TCPServer::startAccept() {
        _socket.emplace(_ioContext);

        //asynchronously accept a connection
        _acceptor.async_accept(*_socket,[this](const boost::system::error_code& error){
            auto connection = TCPConnection::Create(std::move(*_socket));

            if(OnJoin){
                OnJoin(connection);
            }

            _connections.insert(connection);
            if(!error){
                connection->start(
                        [this](const std::string& message){if(OnClientMessage) OnClientMessage(message);},
                        [&, weak=std::weak_ptr(connection)]{
                            if(auto shared = weak.lock(); shared && _connections.erase(shared)){
                                if(OnLeave) OnLeave(shared);
                            }
                        }
                        );
            }else{

            }
            startAccept();
        });
    }


}
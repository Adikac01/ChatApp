#include <iostream>
#include "ChatNetworking/server/TCPServer.h"
#include "ChatNetworking/server/TcpConnection.h"
#include <memory>
#include <utility>


namespace Chat {
    using asio::ip::tcp;

    TCPServer::TCPServer(IPV ipv, int port) : _ipVersion(ipv), _port(port),
    _acceptor(_ioContext,tcp::endpoint(_ipVersion == IPV::V4 ? tcp::v4() : tcp::v6(),_port)) {
        _generalRoom = TCPChatRoom::createRoom("\\General\\");
        _chatRooms.emplace(_generalRoom);
    }

    void TCPServer::makeNewRoom(std::string name)
    {
        _newRoom = TCPChatRoom::createRoom(name);
        _chatRooms.emplace(_newRoom);

    }

    int TCPServer::run() {
        try {
            startAccept();
            _ioContext.run();
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    void TCPServer::Broadcast(const std::string& message, const TCPChatRoom::pointer &ptr) {
        if(ptr == nullptr){
            _generalRoom->Broadcast(message);
        }else{
            ptr->Broadcast(message);
        }
    }

    void TCPServer::Commands(const std::string &message) {
        //TODO


    }


    void TCPServer::startAccept() {
        _socket.emplace(_ioContext);

        //asynchronously accept a connection
        _acceptor.async_accept(*_socket, [this](const boost::system::error_code &error) {
            auto connection = TCPConnection::Create(std::move(*_socket));
            if (OnJoin) {
                OnJoin(connection);
            }


            _generalRoom->addConnection(connection);
            if (!error) {
                connection->start(
                        [this](const std::string &message)
                        //TODO: Change function to work with chatRoom pointer
                        { if (OnClientMessage) OnClientMessage(message); },
                        [&, weak = std::weak_ptr(connection)] {
                            //TODO: Make connections from chatrooms
                            if (auto shared = weak.lock(); shared && _connections.erase(shared)) {
                                if (OnLeave) OnLeave(shared);
                            }
                        },
                        [this](const std::string &message, TCPConnection::pointer ptr) {
                            //TODO: Change function to work with chatRoom pointer
                            if (OnUsernameSet) OnUsernameSet(message, std::move(ptr));
                        },
                        [this]() {
                            std::vector<std::string> users{};
                            for (const auto &chatRoom: _chatRooms) chatRoom->GetUsers(users);
                            return users;
                        }

                );
            } else {

            }
            startAccept();
        });
    }


}
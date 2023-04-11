#include <iostream>
#include "ChatNetworking/server/TCPServer.h"
#include "ChatNetworking/server/TcpConnection.h"
#include <memory>
#include <utility>


namespace Chat {
    using asio::ip::tcp;

    TCPServer::TCPServer(IPV ipv, int port) : _ipVersion(ipv), _port(port),
    _acceptor(_ioContext,tcp::endpoint(_ipVersion == IPV::V4 ? tcp::v4() : tcp::v6(),_port)) {
        _generalRoom = TCPChatRoom::createRoom("General");
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
        if(ptr) {
            ptr->Broadcast(message);
        }else{
            std::cout << ptr;
        }
    }

    void TCPServer::Commands(const std::string &message) {
        //TODO


    }


    void TCPServer::startAccept() {
        _socket.emplace(_ioContext);

        //asynchronously accept a connection
        _acceptor.async_accept(*_socket, [this](const boost::system::error_code &error) {
            auto connection = TCPConnection::Create(std::move(*_socket),std::weak_ptr(_generalRoom));
            if (OnJoin) {
                OnJoin(connection);
            }


            _generalRoom->addConnection(connection);
            if (!error) {
                connection->start(
                        [this](const std::string &message, const std::weak_ptr<TCPChatRoom>& chatRoom)
                        {
                            auto shared = chatRoom.lock();
                            if (OnClientMessage) OnClientMessage(message, shared);
                            },
                        [&, weak = std::weak_ptr(connection)](const std::weak_ptr<TCPChatRoom>& chatRoom) {
                            if (auto shared = weak.lock(); shared && _connections.erase(shared)) {
                                auto room = chatRoom.lock();
                                if (OnLeave) OnLeave(shared, room);
                            }
                        },
                        [this](const std::string &message, const std::weak_ptr<TCPChatRoom>& chatRoom) {
                            auto shared = chatRoom.lock();
                            if (OnUsernameSet) OnUsernameSet(message,shared);
                        },
                        [this]() {
                            std::vector<std::string> users{};
                            for (const auto &chatRoom: _chatRooms) chatRoom->GetUsers(users);
                            return users;
                        },
                        [this](const std::string& name){
                            _newRoom = TCPChatRoom::createRoom(name);
                            _chatRooms.emplace(_newRoom);
                            std::cout<<"room created "<<name;
                        },[&,weak = std::weak_ptr(connection)](const std::string& name,
                                        const std::weak_ptr<TCPChatRoom>& chatRoom){

                            auto weaker = weak.lock();
                            auto shared = chatRoom.lock();


                            for(const auto& room : _chatRooms)
                            {
                                if(room->getName()==name)
                                {
                                    room->addConnection(weaker);
                                    //Delete connection from previous room only if we found another one;
                                    if(shared) {
                                        shared->DelConnection(weaker);
                                    }
                                    return std::weak_ptr(room);
                                }
                            }
                            //If room with that name wasn't found, return previous room pointer;
                            return chatRoom;
                        }

                );
            } else {

            }
            startAccept();
        });
    }


}
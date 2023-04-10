#include <utility>

#include "ChatNetworking/server/TcpChatRoom.h"
#include <ChatNetworking/server/TcpConnection.h>

namespace Chat {
    TCPChatRoom::TCPChatRoom(std::string name,TCPConnection::pointer&& ptr):_chatName(std::move(name)){
        addConnection(ptr);
    }

    void TCPChatRoom::addConnection(const TCPConnection::pointer &ptr) {
        if(ptr != nullptr) _connections.emplace(ptr);
    }

    void TCPChatRoom::DelConnection(const TCPConnection::pointer& connection)
    {
        _connections.erase(connection);
    }

    void TCPChatRoom::GetUsers(std::vector<std::string> &users) {
        for(const auto &connection : _connections)
            if (connection->checkUsernameInitialized()) {
                std::string user;
                user += R"(\\\)" + this->getName() + ": " + connection->getUsername();
                users.push_back(user);
            }
    }

    void TCPChatRoom::Broadcast(const std::string& message){
        for(const auto& connection : _connections){
            connection->post(message);
        }
    }


} // Chat
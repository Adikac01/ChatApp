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

    void TCPChatRoom::GetUsers(std::vector<std::string> &users) {
        for(const auto &connection : this->getChatConnections())
            if (connection->checkUsernameInitialized()) {
                std::string user;
                user += R"(\\\)" + this->getName() + ": ";
                users.push_back(connection->getUsername());
            }
    }


} // Chat
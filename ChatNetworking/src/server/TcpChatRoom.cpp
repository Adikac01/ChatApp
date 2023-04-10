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


} // Chat
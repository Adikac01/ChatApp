#ifndef CHATAPP_TCPCHATROOM_H
#define CHATAPP_TCPCHATROOM_H

#include <boost/asio.hpp>
#include <ChatNetworking/server/TcpConnection.h>
#include <unordered_set>

namespace Chat {
    class TCPConnection;

    class TCPChatRoom {
    public:
        using pointer = std::shared_ptr<TCPChatRoom>;
        static pointer createRoom(std::string name, TCPConnection::pointer &&connection = nullptr) {
            return pointer(new TCPChatRoom(std::move(name), std::move(connection)));
        }
        void addConnection(const TCPConnection::pointer &ptr);

        std::unordered_set<TCPConnection::pointer> getChatConnections() { return _connections; }

        std::string getName() { return _chatName; }

        void GetUsers(std::vector<std::string> &users);

        void Broadcast(const std::string& message);

    private:
        TCPChatRoom(std::string name, TCPConnection::pointer&& ptr);
    private:
        std::string _chatName;
        std::unordered_set<TCPConnection::pointer> _connections;

    };

} // Chat

#endif //CHATAPP_TCPCHATROOM_H

#ifndef CHATAPP_TCPSERVER_H
#define CHATAPP_TCPSERVER_H

#include <boost/asio.hpp>
#include <ChatNetworking/server/TcpChatRoom.h>
#include <functional>
#include <optional>
#include <unordered_set>
#include <vector>


namespace Chat {
    enum class IPV {
        V4,
        V6
    };
    using asio::ip::tcp;

    class TCPServer {
        using OnJoinHandler = std::function<void(TCPConnection::pointer)>;
        using OnLeaveHandler = std::function<void(TCPConnection::pointer, TCPChatRoom::pointer)>;
        using OnClientMessageHanler = std::function<void(std::string, TCPChatRoom::pointer)>;
        using OnUsernameSetHandler = std::function<void(std::string, TCPChatRoom::pointer)>;
        using OnChatCreatedHandler = std::function<void(std::string)>;

    public:
        //Constructors
        TCPServer(IPV ipv, int port);

        //Functions
        int run();

        void Commands(const std::string &message);

        void Broadcast(const std::string &message, const TCPChatRoom::pointer &ptr = nullptr);

        OnJoinHandler OnJoin;
        OnLeaveHandler OnLeave;
        OnClientMessageHanler OnClientMessage;
        OnUsernameSetHandler OnUsernameSet;
        OnChatCreatedHandler OnChatCreated;

    private:
        void startAccept();

    private:
        IPV _ipVersion;
        int _port;

        asio::io_context _ioContext;
        tcp::acceptor _acceptor;
        std::optional<tcp::socket> _socket;
        std::unordered_set<TCPConnection::pointer> _connections{};
        std::unordered_set<TCPChatRoom::pointer> _chatRooms{};
        TCPChatRoom::pointer _generalRoom;
    };
}//Chat

#endif //CHATAPP_TCPSERVER_H

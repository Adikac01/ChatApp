#ifndef CHATAPP_TCPSERVER_H
#define CHATAPP_TCPSERVER_H
#include <boost/asio.hpp>
#include "TcpConnection.h"
#include <functional>
#include <optional>
#include <unordered_set>


namespace Chat {
    enum class IPV{
        V4,
        V6
    };
    using asio::ip::tcp;
    class TCPServer {
        using OnJoinHandler = std::function<void(TCPConnection::pointer)>;
        using OnLeaveHandler = std::function<void(TCPConnection::pointer)>;
        using OnClientMessageHanler = std::function<void(std::string)>;

    public:
        //Constructors
        TCPServer(IPV ipv, int port);

        //Functions
        int run();

        void Broadcast(const std::string &message);

        OnJoinHandler OnJoin;
        OnLeaveHandler OnLeave;
        OnClientMessageHanler OnClientMessage;

    private:
        void startAccept();

    private:
        IPV _ipVersion;
        int _port;

        asio::io_context _ioContext;
        tcp::acceptor _acceptor;
        std::optional<tcp::socket> _socket;
        std::unordered_set<TCPConnection::pointer> _connections {};
    };
}//Chat

#endif //CHATAPP_TCPSERVER_H
#ifndef CHATAPP_TCPCONNECTION_H
#define CHATAPP_TCPCONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include <queue>


namespace Chat {

    namespace asio = boost::asio;
    namespace sys = boost::system;
    using asio::ip::tcp;


    class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
    public:
        using pointer = std::shared_ptr<TCPConnection>;
        using UsernameHandler = std::function<void(std::string, pointer)>;
        using MessageHandler = std::function<void(std::string)>;
        using ErrorHandler = std::function<void()>;
        using AllConnectionsHandler = std::function<std::vector<std::string>()>;

        static pointer Create(tcp::socket &&socket) {
            //return std::make_shared<TCPConnection>(ioContext);
            return pointer(new TCPConnection(std::move(socket)));
        }

        inline const std::string &getUsername() const { return _username; }

        tcp::socket &getSocket() {
            return _socket;
        }


        void start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler,
                   UsernameHandler &&usernameHandler, AllConnectionsHandler &&connectionsHandler);

        void post(const std::string &message);

        void getStarted();

        bool checkUsernameInitialized() const { return _usernameInitialized; }

        void initializeName() { _usernameInitialized = true;}


    private:

        explicit TCPConnection(tcp::socket &&socket);

        void asyncRead();

        void onRead(sys::error_code error, size_t bytesTransferred);

        void asyncWrite();

        void onWrite(sys::error_code error, size_t bytesTransferred);

        void readUsername();

        void onUsernameSet();


    private:
        tcp::socket _socket;
        std::string _username;
        bool _usernameInitialized = false;
        std::weak_ptr<class TCPChatRoom> _chatRoom;

        std::queue<std::string> _outgoingMessages;
        asio::streambuf _streamBuf{65536};

        MessageHandler _messageHandler;
        ErrorHandler _errorHandler;
        UsernameHandler _usernameHandler;
        AllConnectionsHandler _connectionsHandler;


    };
} // Chat

#endif //CHATAPP_TCPCONNECTION_H

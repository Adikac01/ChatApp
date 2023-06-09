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
        using chatPointer = std::weak_ptr<class TCPChatRoom>;
        using UsernameHandler = std::function<void(std::string, chatPointer)>;
        using MessageHandler = std::function<void(std::string,chatPointer)>;
        using ErrorHandler = std::function<void(const std::weak_ptr<TCPChatRoom>& chatRoom)>;
        using AllConnectionsHandler = std::function<std::vector<std::string>()>;
        using ChatCreateHandler = std::function<bool(std::string)>;
        using ChatJoinHandler = std::function<chatPointer(std::string,const chatPointer& chatRoom,bool isLeave)>;



        static pointer Create(tcp::socket &&socket, std::weak_ptr<TCPChatRoom> weak) {
            //return std::make_shared<TCPConnection>(ioContext);
            return pointer(new TCPConnection(std::move(socket),weak));
        }

        inline const std::string &getUsername() const { return _username; }

        tcp::socket &getSocket() {
            return _socket;
        }


        void start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler,
                   UsernameHandler &&usernameHandler, AllConnectionsHandler &&connectionsHandler,
                   ChatCreateHandler &&chatCreateHandler,ChatJoinHandler &&chatJoinHandler);

        void post(const std::string &message);

        void getStarted();

        bool checkUsernameInitialized() const { return _usernameInitialized; }

        void initializeName() { _usernameInitialized = true;}

    private:

        explicit TCPConnection(tcp::socket &&socket, std::weak_ptr<TCPChatRoom> &weak);

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
        ChatCreateHandler _chatCreateHandler;
        ChatJoinHandler _chatJoinHandler;
    };
} // Chat

#endif //CHATAPP_TCPCONNECTION_H

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
        static pointer Create(tcp::socket&& socket){
            //return std::make_shared<TCPConnection>(ioContext);
            return pointer(new TCPConnection(std::move(socket)));
        }
        tcp::socket& getSocket(){
            return _socket;
        }

        void start();
        void post(const std::string& message);


    private:

        explicit TCPConnection(tcp::socket&& socket);

        void asyncRead();
        void onRead(sys::error_code error, size_t bytesTransferred);

        void asyncWrite();
        void onWrite(sys::error_code error, size_t bytesTransferred);


    private:
        tcp::socket _socket;
        std::string _username;

        std::queue<std::string> _outgoingMessages;
        asio::streambuf _streamBuf {65536};
    };
} // Chat

#endif //CHATAPP_TCPCONNECTION_H

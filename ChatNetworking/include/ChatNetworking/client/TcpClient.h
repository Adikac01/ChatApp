#ifndef CHATAPP_TCPCLIENT_H
#define CHATAPP_TCPCLIENT_H

#include <boost/asio.hpp>
#include <queue>

namespace Chat {
    namespace asio = boost::asio;
    namespace sys = boost::system;
    using asio::ip::tcp;
    using MessageHandler = std::function<void(std::string)>;
    class TCPClient {
    public:
        TCPClient(const std::string& address, int port);

        void run();
        void stop();
        void Post(const std::string& message);

    private:
        void asyncRead();
        void onRead(sys::error_code error, size_t bytesTransferred);


        void asyncWrite();
        void onWrite(sys::error_code error, size_t bytesTransferred);

    public:
        MessageHandler OnMessage;

    private:
        asio::io_context _ioContext {};
        tcp::socket _socket;

        tcp::resolver::results_type _endpoints;
        asio::streambuf _streamBuf{65536};
        std::queue<std::string> _outgoingMessages{};
    };

} // Chat

#endif //CHATAPP_TCPCLIENT_H

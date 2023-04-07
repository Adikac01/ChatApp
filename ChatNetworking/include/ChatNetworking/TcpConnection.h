#ifndef CHATAPP_TCPCONNECTION_H
#define CHATAPP_TCPCONNECTION_H
#include <boost/asio.hpp>
#include <memory>


namespace Chat {
    namespace asio = boost::asio;
    using asio::ip::tcp;
    class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
    public:
        using pointer = std::shared_ptr<TCPConnection>;
        static pointer Create(asio::io_context& ioContext){
            //return std::make_shared<TCPConnection>(ioContext);
            return pointer(new TCPConnection(ioContext));
        }
        tcp::socket& getSocket(){
            return _socket;
        }

        void Start();

    private:

        explicit TCPConnection(asio::io_context& ioContext);

    private:
        tcp::socket _socket;
        std::string _message {"Hello there, old sport\n"};
    };
} // Chat

#endif //CHATAPP_TCPCONNECTION_H

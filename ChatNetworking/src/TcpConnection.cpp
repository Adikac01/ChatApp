#include <iostream>
#include "ChatNetworking/TcpConnection.h"


namespace Chat {
    TCPConnection::TCPConnection(tcp::socket&& socket) : _socket(std::move(socket)) {
        boost::system::error_code error;
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }

    void TCPConnection::Start() {
        asyncRead();
    }

    void TCPConnection::Post(const std::string &message) {

    }

    void TCPConnection::asyncRead() {
        asio::async_read_until(_socket, _streamBuf, "\n",
                               [self = shared_from_this()] (sys::error_code error, size_t bytesTransferred){
           self->onRead(error,bytesTransferred);
        });
    }


    void TCPConnection::onRead(sys::error_code error, size_t bytesTransferred) {
        if(error){
            _socket.close();

            //add an error handler
            return;
        }

        std::stringstream msg;
        msg << _username << ": " << std::istream(&_streamBuf).rdbuf();
        _streamBuf.consume(bytesTransferred);

        std::cout << msg.str() << std::endl;

        asyncRead();
    }

    void TCPConnection::asyncWrite() {

    }

    void TCPConnection::onWrite(sys::error_code error, size_t bytesTransferred) {

    }


} // Chat
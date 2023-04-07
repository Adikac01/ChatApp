#include <iostream>
#include "ChatNetworking/TcpConnection.h"


namespace Chat {
    TCPConnection::TCPConnection(tcp::socket&& socket) : _socket(std::move(socket)) {
        boost::system::error_code error;
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }

    void TCPConnection::start() {
        asyncRead();
    }

    void TCPConnection::post(const std::string &message) {
        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);

        if(queueIdle){
            asyncWrite();
        }
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
        std::time_t localTime = std::time(nullptr);
        tm *tmTime = std::localtime(&localTime);
        auto resolveTime = [](int time){
            std::stringstream buf;
            time < 10 ? buf << "0" << time : buf << time;
            return buf.str();
        };
        msg << "[" << resolveTime(tmTime->tm_hour) << ":" << resolveTime(tmTime->tm_min) << ":" << resolveTime(tmTime->tm_sec)
            << "] " << _username << ": " << std::istream(&_streamBuf).rdbuf();
        _streamBuf.consume(bytesTransferred);

        std::cout << msg.str();

        asyncRead();
    }

    void TCPConnection::asyncWrite() {
        asio::async_write(_socket, asio::buffer(_outgoingMessages.front()),
                          [self = shared_from_this()] (sys::error_code error, size_t bytesTransferred){
                                   self->onWrite(error,bytesTransferred);
                               });
    }

    void TCPConnection::onWrite(sys::error_code error, size_t bytesTransferred) {
        if(error){
            _socket.close();

            //add an error handler
            return;
        }
        _outgoingMessages.pop();
        if(!_outgoingMessages.empty()){
            asyncWrite();
        }
    }


} // Chat
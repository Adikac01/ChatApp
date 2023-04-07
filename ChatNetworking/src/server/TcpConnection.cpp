#include <iostream>
#include "ChatNetworking/server/TcpConnection.h"


namespace Chat {
    TCPConnection::TCPConnection(tcp::socket&& socket) : _socket(std::move(socket)) {
        boost::system::error_code error;
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }

    void TCPConnection::start(MessageHandler&& messageHandler,ErrorHandler&& errorHandler) {
        _messageHandler = std::move(messageHandler);
        _errorHandler = std::move(errorHandler);
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


            _errorHandler();
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
        auto timeBuffer= [&resolveTime](tm* t){
            std::stringstream buffer;
            buffer << "[" << resolveTime(t->tm_hour) << ":" << resolveTime(t->tm_min) << ":" << resolveTime(t->tm_sec)  << "] " ;
            return buffer.str();
        };
        msg << timeBuffer(tmTime)<< _username << ": " << std::istream(&_streamBuf).rdbuf();
        _streamBuf.consume(bytesTransferred);

        std::cout << msg.str();

        _messageHandler(msg.str());
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

            _errorHandler();
            return;
        }
        _outgoingMessages.pop();
        if(!_outgoingMessages.empty()){
            asyncWrite();
        }
    }


} // Chat
#include <iostream>
#include "ChatNetworking/server/TcpConnection.h"
#include <windows.h>


namespace Chat {
    TCPConnection::TCPConnection(tcp::socket&& socket) : _socket(std::move(socket)) {
        boost::system::error_code error;
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }

    void TCPConnection::getStarted() {
        asio::async_write(_socket, asio::buffer("Set your username:\n"),
                          [self = shared_from_this()] (sys::error_code error, size_t bytesTransferred){
                              if(!error){
                                  self->readUsername();
                              }
                          });
    }

    void TCPConnection::readUsername() {
        asio::async_read(_socket, _streamBuf, asio::transfer_at_least(1),
                         [self = shared_from_this()](const sys::error_code& error, size_t bytesTransferred){
                             if(!error){
                                 std::string username = asio::buffer_cast<const char*>(self->_streamBuf.data());
                                 username = username.substr(0, username.size()-1); // remove trailing newline
                                 self->_username = username;
                                 self->_streamBuf.consume(bytesTransferred);
                                 self->onUsernameSet();
                                 self->asyncRead();
                             }
                         });
    }

    void TCPConnection::onUsernameSet() {
        std::string msg = "User " + _username + " has joined the chat!\n";
        _usernameHandler(msg, shared_from_this());
    }

    void TCPConnection::start(MessageHandler&& messageHandler,ErrorHandler&& errorHandler,
                              UsernameHandler&& usernameHandler, AllConnectionsHandler&& connectionsHandler) {
        _messageHandler = std::move(messageHandler);
        _errorHandler = std::move(errorHandler);
        _usernameHandler = std::move(usernameHandler);
        _connectionsHandler = std::move(connectionsHandler);
        getStarted();
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
//
//        std::basic_streambuf<char> *message= (std::istream(&_streamBuf).rdbuf());
//        std::cout<<&message<<std::endl;
        std::stringstream message;
        message << std::istream(&_streamBuf).rdbuf();
        std::cout << message.str();
        std::string msg_str = message.str();
        msg_str.substr(0,msg_str.size()-1);

        if(msg_str[0]=='\\')
        {
            if(msg_str=="\\users\n")
            {
                std::vector<std::string> users = _connectionsHandler();

                std::string tmp;
                for(const auto& user : users)
                {
                    std::cout<<user<<std::endl;
                    tmp+=user+"\n";

                }
                std::cout<<users.size();

                asio::async_write(_socket, asio::buffer(tmp),
                                  [self = shared_from_this()] (sys::error_code error, size_t bytesTransferred){
                                      if(!error){
                                      }
                                  });

            }
            asyncRead();
        }else {

            std::stringstream msg;
            std::time_t localTime = std::time(nullptr);
            tm *tmTime = std::localtime(&localTime);
            auto resolveTime = [](int time) {
                std::stringstream buf;
                time < 10 ? buf << "0" << time : buf << time;
                return buf.str();
            };
            auto timeBuffer = [&resolveTime](tm *t) {
                std::stringstream buffer;
                buffer << "[" << resolveTime(t->tm_hour) << ":" << resolveTime(t->tm_min) << ":"
                       << resolveTime(t->tm_sec) << "] ";
                return buffer.str();
            };
            msg << timeBuffer(tmTime) << _username << ": " << message.str();

            _streamBuf.consume(bytesTransferred);

            std::cout << msg.str();

            _messageHandler(msg.str());
            asyncRead();

        }
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
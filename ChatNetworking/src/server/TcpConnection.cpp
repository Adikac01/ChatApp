#include <iostream>
#include "ChatNetworking/server/TcpConnection.h"


namespace Chat {
    TCPConnection::TCPConnection(tcp::socket &&socket) : _socket(std::move(socket)) {
        boost::system::error_code error;
        std::stringstream name;
        name << _socket.remote_endpoint();
        _username = name.str();
    }

    void TCPConnection::getStarted() {
        asio::async_write(_socket, asio::buffer("Set your username:\n"),
                          [self = shared_from_this()](sys::error_code error, size_t bytesTransferred) {
                              if (!error) {
                                  self->readUsername();
                              }
                          });
    }

    void TCPConnection::readUsername() {
        asio::async_read(_socket, _streamBuf, asio::transfer_at_least(1),
                         [self = shared_from_this()](const sys::error_code &error, size_t bytesTransferred) {
                             if (!error) {
                                 std::string username = asio::buffer_cast<const char *>(self->_streamBuf.data());
                                 username = username.substr(0, username.size() - 1); // remove trailing newline
                                 self->_username = username;
                                 self->_streamBuf.consume(bytesTransferred);
                                 self->onUsernameSet();
                                 self->asyncRead();
                             }
                         });
    }

    void TCPConnection::onUsernameSet() {
        std::string msg = "User " + _username + " has joined the chat!\n";
        _usernameHandler(msg, _chatRoom);
        initializeName();
    }

    void TCPConnection::start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler,
                              UsernameHandler &&usernameHandler, AllConnectionsHandler &&connectionsHandler,
                              ChatCreateHandler &&chatCreateHandler) {
        _messageHandler = std::move(messageHandler);
        _errorHandler = std::move(errorHandler);
        _usernameHandler = std::move(usernameHandler);
        _connectionsHandler = std::move(connectionsHandler);
        _chatCreateHandler = std::move(chatCreateHandler);
        getStarted();
    }

    void TCPConnection::post(const std::string &message) {
        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);
        if (queueIdle) {
            asyncWrite();
        }
    }

    void TCPConnection::asyncRead() {
        asio::async_read_until(_socket, _streamBuf, "\n",
                               [self = shared_from_this()](sys::error_code error, size_t bytesTransferred) {
                                   self->onRead(error, bytesTransferred);
                               });
    }


    void TCPConnection::onRead(sys::error_code error, size_t bytesTransferred) {
        if (error) {
            _socket.close();

            _errorHandler(_chatRoom);
            return;
        }
        std::stringstream message;
        message << std::istream(&_streamBuf).rdbuf();
        std::string msg_str = message.str();
        std::string cmd;
        size_t i = 0;
        while(i < msg_str.size() -1 && msg_str[i] != ' ' && msg_str[i] != '\n'){
            cmd += msg_str[i++];
        }
        msg_str = msg_str.substr(std::min((i+1),(msg_str.size()-1)), std::max(i, msg_str.size() -2));

        if (cmd[0] == '/') {
            if (cmd == "/users") {
                std::vector<std::string> users = (_connectionsHandler());

                std::string tmp = "List of users:\n";
                for (const auto &user: users) {
                    tmp += user + "\n";
                }

                asio::async_write(_socket, asio::buffer(tmp),
                                  [self = shared_from_this()](sys::error_code error, size_t bytesTransferred) {
                                      if (!error) {
                                      }
                                  });

            }else if (cmd == "/create_room"){
                //TODO: Implement logic
            }
            asyncRead();
        } else {

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

            _messageHandler(msg.str(), _chatRoom);
            asyncRead();

        }
    }

    void TCPConnection::asyncWrite() {
        asio::async_write(_socket, asio::buffer(_outgoingMessages.front()),
                          [self = shared_from_this()](sys::error_code error, size_t bytesTransferred) {
                              self->onWrite(error, bytesTransferred);
                          });
    }

    void TCPConnection::onWrite(sys::error_code error, size_t bytesTransferred) {
        if (error) {
            _socket.close();

            _errorHandler(_chatRoom);
            return;
        }
        _outgoingMessages.pop();
        if (!_outgoingMessages.empty()) {
            asyncWrite();
        }
    }


} // Chat
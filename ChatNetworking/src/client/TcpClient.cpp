#include <iostream>
#include "ChatNetworking/client/TcpClient.h"

namespace Chat {

    TCPClient::TCPClient(const std::string &address, int port) : _socket(_ioContext) {
        asio::ip::tcp::resolver resolver{_ioContext};
        _endpoints = resolver.resolve(address, std::to_string(port));
    }

    void TCPClient::run() {
        asio::async_connect(_socket, _endpoints, [this](sys::error_code error, tcp::endpoint ep) {
            if (!error) {
                asyncRead();
            }
        });

        _ioContext.run();
    }

    void TCPClient::stop() {
        sys::error_code error;
        _socket.close(error);

        if (error) {
            //process error
        }
    }

    void TCPClient::Post(const std::string &message) {
        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);
        if (queueIdle) {
            asyncWrite();
        }
    }

    void TCPClient::asyncRead() {
        asio::async_read_until(_socket, _streamBuf, "\n", [this](sys::error_code error, size_t bytesTransferred) {
            onRead(error, bytesTransferred);
        });
    }

    void TCPClient::onRead(sys::error_code error, size_t bytesTransferred) {
        if (error) {
            stop();
            return;
        }

        std::stringstream message;
        message << std::istream(&_streamBuf).rdbuf();
        OnMessage(message.str());
        asyncRead();
    }

    void TCPClient::asyncWrite() {
        asio::async_write(_socket, asio::buffer(_outgoingMessages.front()),
                          [this](sys::error_code error, size_t bytesTransferred) {
                              onWrite(error, bytesTransferred);
                          });
    }

    void TCPClient::onWrite(sys::error_code error, size_t bytesTransferred) {
        if (error) {
            stop();
            return;
        }

        _outgoingMessages.pop();

        if (!_outgoingMessages.empty()) {
            asyncWrite();
        }
    }
} // Chat
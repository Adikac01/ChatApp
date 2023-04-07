#include <iostream>
#include "ChatNetworking/TcpConnection.h"


namespace Chat {
    TCPConnection::TCPConnection(asio::io_context &ioContext) : _socket(ioContext) {

    }

    void TCPConnection::Start() {
        auto strongThis = shared_from_this();
        asio::async_write(_socket, asio::buffer(_message),
                                 [strongThis](const boost::system::error_code& error,size_t bytesTransferred){
             if(error){
                 std::cout << "Failed to load message\n";
             }else{
                 std::cout << "Sent " << bytesTransferred << " bytes of data\n";
             }

         });
        asio::streambuf buffer;
        _socket.async_receive(buffer.prepare(512),
                              [this](const boost::system::error_code& error,size_t bytesTransferred){
          if(error == asio::error::eof){
              //clean connection cut off
              std::cout << "Client disconnected properly";
          }else if(error){
              std::cout << "Client disconnected in bad way";
          }
      });
    }


} // Chat
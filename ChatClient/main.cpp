#include <iostream>
#include <boost/asio.hpp>
#include <array>
#include <ChatNetworking/client/TCPClient.h>

namespace asio = boost::asio;
namespace sys = boost::system;
using asio::ip::tcp;


int main(int argc, char* argv[]) {
    try{
        asio::io_context ioContext;
        tcp::resolver resolver {ioContext};

        auto endpoints = resolver.resolve("127.0.0.1", "1337");

        tcp::socket socket{ioContext};
        asio::connect(socket, endpoints);

        while(true){
            //Listen for messages
            std::array<char, 128> buf {};
            sys::error_code error;

            size_t len = socket.read_some(asio::buffer(buf),error);

            if(error == asio::error::eof){
                //clean connection cut off
                break;
            }else if(error){
                throw sys::system_error(error);
            }

            std::cout.write(buf.data(), len);
        }

    }catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

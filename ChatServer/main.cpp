#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
int main() {
    try {
        boost::asio::io_context ioContext;

        tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), 1337));

        while (true) {
            std::cout << "Accepting connections on port 1337\n";

            tcp::socket socket(ioContext);
            acceptor.accept(socket);

            std::string helloMessage = "Hello dear, old sport\n";
            boost::system::error_code error;

            boost::asio::write(socket, boost::asio::buffer(helloMessage), error);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
   }
    return 0;
}
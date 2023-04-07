#ifndef CHATAPP_TCPCLIENT_H
#define CHATAPP_TCPCLIENT_H

#include <boost/asio.hpp>

namespace Chat {
    namespace asio = boost::asio;
    namespace sys = boost::system;
    using MessageHandler = std::function<void(std::string)>;
    class TCPClient {
    public:
        TCPClient(const std::string& address, int port);

        void run();
        void stop();
        void Post(const std::string& message);

    private:
        void asyncRead();
        void onRead(sys::error_code error, size_t bytesTransferred);


        void asyncWrite();
        void onWrite(sys::error_code error, size_t bytesTransferred);
    };

} // Chat

#endif //CHATAPP_TCPCLIENT_H

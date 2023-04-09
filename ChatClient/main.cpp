#include <ChatNetworking/client/TCPClient.h>
#include <iostream>
#include <thread>

using namespace Chat;


int main(int argc, char *argv[]) {
    TCPClient client{"localhost", 1337};

    client.OnMessage = [](const std::string &message) {
        std::cout << message;
    };

    std::thread t{[&client]() { client.run(); }};

    while (true) {
        std::string message;
        std::getline(std::cin, message);

        if (message == "\\q") break;

        message += '\n';

        client.Post(message);
    }

    client.stop();
    t.join();
    return 0;
}

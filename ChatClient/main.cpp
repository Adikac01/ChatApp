#include <iostream>

#include <ChatNetworking/Base.h>

int main() {
    std::cout << "Hello, Client! " << static_cast<int>(MyEnum::One)  << std::endl;
    return 0;
}

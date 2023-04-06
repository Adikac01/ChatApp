#include <iostream>

#include "ChatNetworking/Base.h"

int main() {
    std::cout << "Hello, Server! " << static_cast<int>(MyEnum::Two) << std::endl;
    return 0;
}
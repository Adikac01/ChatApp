cmake_minimum_required(VERSION 3.25)
project(ChatClient)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp ../ChatNetworking/include/ChatNetworking/Base.h ../ChatNetworking/src/Base.cpp)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            ChatNetworking
            ws2_32
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ChatNetworking
        )

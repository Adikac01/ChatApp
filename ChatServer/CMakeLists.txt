cmake_minimum_required(VERSION 3.25)
project(ChatServer)

set(CMAKE_CXX_STANDARD 17)



add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ChatNetworking
        ws2_32
        mswsock
        PRIVATE
        ws2_32
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ChatNetworking
        )
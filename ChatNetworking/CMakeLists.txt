cmake_minimum_required(VERSION 3.25)
project(ChatNetworking)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.81.0 REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Boost_INCLUDE_DIRS}
        PRIVATE
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${Boost_LIBRARIES}
        ws2_32
        PRIVATE
        ws2_32
        )
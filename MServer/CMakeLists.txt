cmake_minimum_required(VERSION 3.21)
project(MServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            MNetworking
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            MNetworking
        )

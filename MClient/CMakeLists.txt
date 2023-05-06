cmake_minimum_required(VERSION 3.21)
project(MClient)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            MNetworking
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            MNetworking
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
)

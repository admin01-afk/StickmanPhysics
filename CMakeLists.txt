cmake_minimum_required(VERSION 3.15)
project(StickmanRagdoll)

set(CMAKE_CXX_STANDARD 17)

find_package(raylib REQUIRED)
find_package(box2d REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

include_directories(include)

file(GLOB SOURCES src/*.cpp)

add_executable(stickman ${SOURCES})

target_link_libraries(stickman
    PRIVATE raylib box2d JsonCpp::JsonCpp
)
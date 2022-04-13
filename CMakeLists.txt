cmake_minimum_required(VERSION 3.21)
project(PollerAsio)

set(CMAKE_CXX_STANDARD 14)

add_executable(PollerAsio main.cpp Poller.cpp Poller.h Callback.cpp Callback.h)
target_link_libraries(PollerAsio pthread)
cmake_minimum_required(VERSION 3.7)
project(epoll)

set(CMAKE_C_STANDARD 99)

set(SERVER server.c)
set(CLIENT client.c)

add_executable(server ${SERVER})
add_executable(client ${CLIENT})
cmake_minimum_required(VERSION 3.22.1)
project(homework_01)

set(CMAKE_CXX_STANDARD 17)

add_executable(client src/client.cpp src/service/client.cpp src/service/client.h src/service/util.cpp src/service/util.h src/const/const.h)
add_executable(server src/server.cpp src/service/server.cpp src/service/server.h src/service/util.cpp src/service/util.h src/const/const.h)

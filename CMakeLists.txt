cmake_minimum_required (VERSION 2.6)
SET(CMAKE_SYSTEM_NAME Linux)

#SET(CMAKE_FIND_ROOT_PATH "/home/box/computer/arm-linux/arm-2009q3/"
#"/home/box/qt")
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NERVER)
#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MIDE_INCLUDE ONLY)
#
#SET(CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
#SET(CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)
#SET(CMAKE_C_COMPILER arm-emtonix-linux-gnueabi-gcc)
#SET(CMAKE_CXX_COMPILER arm-emtonix-linux-gnueabi-g++)
SET(CMAKE_FIND_ROOT_PATH "/home/ccl/tools/cross_compile/esm6802-linux/sysroots/")

#
#SET(QT_QMAKE_EXECUTABLE /home/box/qt/bin/qmake)

#save current files in pwd into DIR_SRCS
aux_source_directory(. DIR_SRCS)

project(test_tcp_speed_client)


add_executable(test_tcp_speed_client test_tcp_speed_client.cpp TCPClient.h TCPClient.cpp)
#add_executable(test_tcp_speed_client ${DIR_SRCS})

target_link_libraries(test_tcp_speed_client pthread)


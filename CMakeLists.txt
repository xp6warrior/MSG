cmake_minimum_required(VERSION 3.5.0)

project(MSG_Server)
add_executable(MSG_Server MSG_Server/main.c)

project(MSG_Client)
add_executable(MSG_Client MSG_Client/main.c)
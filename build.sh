#!/usr/bin/bash

SERVER_DIR='MSG_Server'
CLIENT_DIR='MSG_Client'

BUILD_DIR='build'

gcc -o ${BUILD_DIR}/server ${SERVER_DIR}/*.c
gcc -o ${BUILD_DIR}/client ${CLIENT_DIR}/*.c
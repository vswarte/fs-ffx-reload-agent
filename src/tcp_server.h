#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "logging.h"
#include "packets.h"
#include "fxr.h"

#pragma comment (lib, "Ws2_32.lib")

// TODO: figure out these parameters
#define RECEIVE_BUFFER_LENGTH 1000000 // 1 megabyte cap
#define DEFAULT_PORT "666"

#define EXCEPTION_STRING_SIZE    1024

// TODO: clean up interface, expose more stuff
class tcp_server {
public:
    void setup();
    void run();
    void cleanup();
private:
    SOCKET _listenSocket = INVALID_SOCKET;
    SOCKET _clientSocket = INVALID_SOCKET;

    void handle_packet(char buffer[RECEIVE_BUFFER_LENGTH], int length, SOCKET socket);
    void handle_fxr_packet(char buffer[RECEIVE_BUFFER_LENGTH], int length, SOCKET socket);
};
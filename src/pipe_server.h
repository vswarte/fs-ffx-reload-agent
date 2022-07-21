#pragma once

#include <winsock2.h>
#include <cstdio>
#include "logging.h"
#include "packets.h"
#include "fxr.h"

#define PIPE_PATH "\\\\.\\pipe\\FSReloader"
#define RECEIVE_BUFFER_LENGTH 1000000 // 1 megabyte cap

class pipe_server {
public:
    void setup();
    void run();
    void cleanup();

private:
    HANDLE _pipe;

    void handle_packet(char buffer[RECEIVE_BUFFER_LENGTH], int length);
    void handle_fxr_packet(char buffer[RECEIVE_BUFFER_LENGTH], int length);
};

#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "logging.h"
#include "tcp_server.h"

void attach();
void detach();

DWORD WINAPI main_thread(void *lpParam);
LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS*);
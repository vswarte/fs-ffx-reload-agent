#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "logging.h"
#include "pipe_server.h"

#define EXCEPTION_STRING_SIZE    1024

void attach();
void detach();

HMODULE current_module;
DWORD WINAPI main_thread(void *lpParam);
LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS*);
#include "entry.h"

BOOL WINAPI DllMain(HMODULE module, DWORD reason, BOOL isStatic) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            attach();
            break;
        case DLL_PROCESS_DETACH:
            detach();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }
    return true;
}

void attach() {
    logging::init();
    logging::write_line("Attached FXR reloader agent");
    SetUnhandledExceptionFilter(reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(exception_handler));
    CreateThread(nullptr, 0x1000000, &main_thread, nullptr, 0, nullptr);
}

void detach() {
    logging::write_line("Detaching FXR reloader agent");
    logging::deinit();
    FreeLibraryAndExitThread(current_module, 0);
}

DWORD main_thread(void *lpParam) {
    logging::write_line("Spinning up main thread");

    auto server = pipe_server();
    server.setup();
    server.run();
    server.cleanup();

    return S_OK;
}

LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS* exception) {
    PEXCEPTION_RECORD record  = exception->ExceptionRecord;
    PCONTEXT context = exception->ContextRecord;
    char exception_text[EXCEPTION_STRING_SIZE];

    switch (record->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            snprintf(
                exception_text,
                EXCEPTION_STRING_SIZE,
                "Access violation!\n\n"
                "Instruction at 0x%p\n"
                "attempted to %s\n"
                "memory at 0x%p\n\n"
                "Register dump:\n"
                "RAX = 0x%p\n" "RBX = 0x%p\n" "RCX = 0x%p\n" "RDX = 0x%p\n"
                "RSI = 0x%p\n" "RDI = 0x%p\n" "RBP = 0x%p\n" "RSP = 0x%p\n"
                "R8  = 0x%p\n" "R9  = 0x%p\n" "R10 = 0x%p\n" "R11 = 0x%p\n"
                "R12 = 0x%p\n" "R13 = 0x%p\n" "R14 = 0x%p\n" "R15 = 0x%p\n",
                record->ExceptionAddress,
                record->ExceptionInformation[0]?"WRITE":"READ",
                record->ExceptionInformation[1],
                context->Rax, context->Rbx, context->Rcx, context->Rdx,
                context->Rsi, context->Rdi, context->Rbp, context->Rsp,
                context->R8,  context->R9,  context->R10, context->R11,
                context->R12, context->R13, context->R14, context->R15
            );
            break;
        default:
            snprintf(
                exception_text,
                EXCEPTION_STRING_SIZE,
                "Caught unhandled exception\n"
                "Code: %x\n"
                "Address: %p",
                record->ExceptionCode,
                record->ExceptionAddress
            );
            break;
    }

    logging::write_line(exception_text);
    MessageBox(NULL, exception_text, "Error Dump", MB_ICONERROR | MB_OK);
    return EXCEPTION_CONTINUE_SEARCH;
}
#include "logging.h"

std::ofstream logging::_fileHandle;

void logging::init() {
    open_log_file();
}

void logging::deinit() {
    close_log_file();
}

void logging::write_line(std::string message) {
    std::ostringstream logEntry;
    logEntry << banner() << message << std::endl;
    log(logEntry.str());
}

void logging::write_line(const char *format, ...) {
    std::ostringstream logEntry;
    char messageBuffer[1000]{};

    va_list args;
    va_start(args, format);
    vsprintf_s(&messageBuffer[0], 1000, format, args);
    va_end(args);

    logEntry << banner() << std::string(messageBuffer) << std::endl;
    log(logEntry.str());
}

void logging::open_log_file() {
    _fileHandle.open("fxr_reloader_agent.log");
}

void logging::close_log_file() {
    if (_fileHandle.is_open()) {
        _fileHandle.close();
    }
}

std::string logging::banner() {
    std::ostringstream banner;
    banner << "[" << get_time() << " FXR-RELOADER] ";
    return banner.str();
}

std::string logging::get_time() {
    time_t rawtime;
    struct tm timeinfo;
    char _buffer[80];

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    strftime(_buffer, sizeof(_buffer), "%H:%M:%S", &timeinfo);
    return { _buffer };
}

void logging::write_to_file(std::string message) {
    if (_fileHandle.is_open()) {
        _fileHandle << message;
        _fileHandle.flush();
    }
}

void logging::log(std::string entry) {
    write_to_file(entry);
}
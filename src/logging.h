#pragma once

#include <ctime>
#include <string>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>
#include <windows.h>

class logging {
public:
    static void init();
    static void deinit();

    static void write_line(std::string message);
    static void write_line(const char* format, ...);
private:
    static void open_log_file();
    static void close_log_file();

    static void log(std::string message);
    static void write_to_file(std::string message);

    static std::string banner();
    static std::string get_time();

    static std::ofstream _fileHandle;
};
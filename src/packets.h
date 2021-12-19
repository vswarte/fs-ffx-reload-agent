#pragma once

#include <cstring>

class base_packet {
public:
    enum class type : char {
        FxrData = 0x1,
    };

    base_packet(char *buffer) { _buffer = buffer; };
protected:
    char *_buffer;

    template <typename T> T read(int offset) {
        auto ptr = _buffer + offset;
        auto length = sizeof(T);

        T temp;
        memcpy(&temp, (void *) ptr, length);
        return temp;
    }
};

class fxr_packet : public base_packet {
public:
    fxr_packet(char *buffer): base_packet(buffer) {}
    int get_size() { return read<int>(0x1); }
};
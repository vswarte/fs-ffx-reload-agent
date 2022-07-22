#pragma once

#include <cstring>

enum class PacketType : char {
    FxrData = 0x1,
    FxrDataResult = 0x2,
};

enum class FxrDataResultStatus : unsigned char
{
    SUCCESS = 0x1,
    INVALID_FXR = 0x2,
    INVALID_NETWORKED_SIZE = 0x3,
};

class base_packet {
public:

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


    template <typename T> void write(int offset, T data) {
        auto ptr = _buffer + offset;
        auto length = sizeof(T);

        memcpy((void *) ptr, &data, length);
    }

public:
    unsigned char* get_packet_buffer();
};

class fxr_data_packet : public base_packet {
public:
    fxr_data_packet(char *buffer) : base_packet(buffer) {}
    int get_size() { return read<int>(0x1); }
};

class fxr_data_result_packet : public base_packet {
public:
    fxr_data_result_packet(char *buffer) : base_packet(buffer) {
        buffer[0] = (unsigned char) PacketType::FxrDataResult;
    }

    void set_fxr_id(int fxrId) { write(0x1, fxrId); }
    void set_status(FxrDataResultStatus status) { write(0x5, status); }
};

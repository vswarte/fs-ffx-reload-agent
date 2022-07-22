#include <thread>
#include <unistd.h>
#include "pipe_server.h"

void pipe_server::setup() {
    logging::write_line("Setting up server");

    _pipe = CreateNamedPipe(TEXT(PIPE_PATH),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024 * 16,
        1024 * 16,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );
}

void pipe_server::run() {
    DWORD receiveLength;
    char receiveBuffer[RECEIVE_BUFFER_LENGTH];

    while (_pipe != INVALID_HANDLE_VALUE) {
        if (ConnectNamedPipe(_pipe, NULL) != FALSE) {
            auto statusThread = std::thread(&pipe_server::status_thread, this);
            logging::write_line("Got pipe connection");
            while (ReadFile(_pipe, receiveBuffer, sizeof(receiveBuffer) - 1, &receiveLength, NULL) != FALSE) {
                logging::write_line("Received message of length %i", receiveLength);
                handle_packet(receiveBuffer, receiveLength);
            }
            statusThread.join();
        }
    }
}

void pipe_server::status_thread() {
    logging::write_line("Cock and ball torture");
    while (true) {
        logging::write_line("What the fuck");
        send_operation_status(-1, FxrDataResultStatus::INVALID_NETWORKED_SIZE);
        sleep(2000);
    }
}

void pipe_server::cleanup() {
    DisconnectNamedPipe(_pipe);
}

void pipe_server::handle_packet(char *buffer, int length) {
    if (length < 1) {
        logging::write_line("cannot handle empty packets");
    }

    auto packetType = *buffer;
    switch (packetType)  {
        case (char) PacketType::FxrData:
            logging::write_line("Received new FXR data");
            handle_fxr_packet(buffer, length);
            break;
        default:
            logging::write_line("Received unknown packet");
            break;
    }
}

void pipe_server::handle_fxr_packet(char *buffer, int length) {
    // TODO: sanity checks and validation
    if (length < 5) {
        logging::write_line("Received invalid FXR packet, length was too little for packet header");
        send_operation_status(-1, FxrDataResultStatus::INVALID_NETWORKED_SIZE);
        return;
    }

    auto fxrPacket = fxr_data_packet(buffer);
    auto fxrSize = fxrPacket.get_size();

    // Check if FXR size corresponds with recv size
    if (length != 0x1 + 0x4 + fxrSize) {
        logging::write_line("Invalid FXR size, specified fxr size %i - recv length %i", fxrSize, length);
        send_operation_status(-1, FxrDataResultStatus::INVALID_NETWORKED_SIZE);
        return;
    }

    logging::write_line("Received FXR, size: %i", fxrSize);

    byte fxrBuffer[fxrSize];
    memcpy(fxrBuffer, buffer + 0x5, fxrSize);

    auto suppliedFxr = *(FXRRoot *) fxrBuffer;
    logging::write_line("Reloading FXR %i", suppliedFxr.ffxId);

    reload_fxr(suppliedFxr.ffxId, (char *) fxrBuffer, fxrSize);
    send_operation_status(suppliedFxr.ffxId, FxrDataResultStatus::SUCCESS);
}

void pipe_server::send_operation_status(int fxrId, FxrDataResultStatus status) {
    char packetBuffer[6];
    auto fxrDataResult = fxr_data_result_packet(packetBuffer);
    fxrDataResult.set_fxr_id(fxrId);
    fxrDataResult.set_status(status);

    logging::write_line("Sending operation status %i", status);

    DWORD bytesWritten;
    auto writeSuccess = WriteFile(_pipe, packetBuffer, sizeof(packetBuffer), &bytesWritten,NULL);
    if (writeSuccess) {
        logging::write_line("bingus %i", bytesWritten);
    } else {
        logging::write_line("big floppa");
    }
}
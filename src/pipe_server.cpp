#include "pipe_server.h"

void pipe_server::setup() {
    logging::write_line("Setting up server");

    _pipe = CreateNamedPipe(TEXT(PIPE_PATH),
        PIPE_ACCESS_DUPLEX,
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
            logging::write_line("Got pipe connection");
            while (ReadFile(_pipe, receiveBuffer, sizeof(receiveBuffer) - 1, &receiveLength, NULL) != FALSE) {
                logging::write_line("Received message of length %i", receiveLength);
                handle_packet(receiveBuffer, receiveLength);
            }
        }
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
        case (char) base_packet::type::FxrData:
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
        return;
    }

    auto fxrPacket = fxr_packet(buffer);
    auto fxrSize = fxrPacket.get_size();

    // Check if FXR size corresponds with recv size
    if (length < 0x1 + 0x4 + fxrSize) {
        logging::write_line("Invalid FXR size, specified fxr size %i - recv length %i", fxrSize, length);
        return;
    }

    logging::write_line("Received FXR, size: %i", fxrSize);

    byte fxrBuffer[fxrSize];
    memcpy(fxrBuffer, buffer + 0x5, fxrSize);

    auto suppliedFxr = *(FXRRoot *) fxrBuffer;
    logging::write_line("Reloading FXR %i", suppliedFxr.ffxId);

    reload_fxr(suppliedFxr.ffxId, (char *) fxrBuffer, fxrSize);
}

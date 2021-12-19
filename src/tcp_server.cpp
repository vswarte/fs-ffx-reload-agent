#include "tcp_server.h"

void tcp_server::setup() {
    logging::write_line("Setting up server");
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        logging::write_line("WSAStartup failed with error: %d", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the tcp_server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        logging::write_line("getaddrinfo failed with error: %d", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to tcp_server
    _listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (_listenSocket == INVALID_SOCKET) {
        logging::write_line("socket failed with error: %ld", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(_listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        logging::write_line("bind failed with error: %d", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(_listenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(_listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        logging::write_line("listen failed with error: %d", WSAGetLastError());
        closesocket(_listenSocket);
        WSACleanup();
        return;
    }

    // Accept a client socket
    _clientSocket = accept(_listenSocket, NULL, NULL);
    if (_clientSocket == INVALID_SOCKET) {
        logging::write_line("accept failed with error: %d", WSAGetLastError());
        closesocket(_listenSocket);
        WSACleanup();
        return;
    }

    closesocket(_listenSocket);
}

void tcp_server::run() {
    int receiveResult;
    char receiverBuffer[RECEIVE_BUFFER_LENGTH];
    int receiveBufferLength = RECEIVE_BUFFER_LENGTH;

    do {
        receiveResult = recv(_clientSocket, receiverBuffer, receiveBufferLength, 0);
        if (receiveResult > 0) {
            handle_packet(receiverBuffer, receiveBufferLength, _clientSocket);
        }
        else if (receiveResult == 0)
            logging::write_line("Connection closing...");
        else  {
            logging::write_line("recv failed with error: %d", WSAGetLastError());
            closesocket(_clientSocket);
            WSACleanup();
            return;
        }
    } while (receiveResult > 0);
}

void tcp_server::cleanup() {
    auto shutdownResult = shutdown(_clientSocket, SD_SEND);
    if (shutdownResult == SOCKET_ERROR) {
        logging::write_line("shutdown failed with error: %d", WSAGetLastError());
        closesocket(_clientSocket);
        WSACleanup();
        return;
    }

    closesocket(_clientSocket);
    WSACleanup();
}

void tcp_server::handle_packet(char *buffer, int length, SOCKET socket) {
    if (length < 1) {
        logging::write_line("cannot handle empty packets");
    }

    auto packetType = *buffer;
    switch (packetType)  {
        case (char) base_packet::type::FxrData:
            logging::write_line("Received new FXR data");
            handle_fxr_packet(buffer, length, socket);
            break;
        default:
            logging::write_line("Received unknown packet");
            break;
    }
}

void tcp_server::handle_fxr_packet(char *buffer, int length, SOCKET socket) {
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
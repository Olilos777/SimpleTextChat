#include "Client.h"

Client::Client() : mainSocket(INVALID_SOCKET) {
    // Inicjalizacja struktury WSADATA
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult;
        throw std::runtime_error("Failed to initialize WSA.");
    }
}

Client::~Client() {
    closesocket(mainSocket);
    WSACleanup();
}

bool Client::Initialize() {

    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET) {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return false;
    }

    return true;
}

bool Client::ConnectToServer() {
    sockaddr_in service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27015);

    if (connect(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("Failed to connect to the server.\n");
        WSACleanup();
        return false;
    }

    return true;
}

void Client::SendAndReceive() {
    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
    char sendbuf[32] = "Client says hello!";
    char recvbuf[32] = "";

    bytesSent = send(mainSocket, sendbuf, strlen(sendbuf), 0);
    printf("Bytes sent: %ld\n", bytesSent);

    while (bytesRecv == SOCKET_ERROR) {
        bytesRecv = recv(mainSocket, recvbuf, 32, 0);

        if (bytesRecv == 0 || bytesRecv == WSAECONNRESET) {
            printf("Connection closed.\n");
            break;
        }

        if (bytesRecv < 0)
            break;

        printf("Bytes received: %ld\n", bytesRecv);
        printf("Received text: %s\n", recvbuf);
    }
}

void Client::SendToServer(const char* message) {
    int bytesSent = send(mainSocket, message, strlen(message), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cout << "Error sending data to server." << std::endl;
    }
}

void Client::HandleChat() {
    char recvbuf[256] = "";
    int bytesRecv = SOCKET_ERROR;

    while (true) {
        bytesRecv = recv(mainSocket, recvbuf, sizeof(recvbuf), 0);
        if (bytesRecv <= 0) {
            std::cout << "Connection closed by the server." << std::endl;
            break;
        }

        std::cout << "Server: " << recvbuf << std::endl;

        // Czyszczenie bufora przed odbiorem kolejnej wiadomoœci
        memset(recvbuf, 0, sizeof(recvbuf));
    }
}

void Client::CloseConnection() {
    closesocket(mainSocket);
    WSACleanup();
}
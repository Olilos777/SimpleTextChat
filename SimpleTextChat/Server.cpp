#include "Server.h"

Server::Server() : mainSocket(INVALID_SOCKET) {
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult;
        throw std::runtime_error("Failed to initialize WSA.");
    }
}

Server::~Server() {
    closesocket(mainSocket);
    WSACleanup();
}

bool Server::Initialize() {
    mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET) {
        printf("Error creating socket: %ld\n", WSAGetLastError());
        WSACleanup();
        return false;
    }

    sockaddr_in service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27015);

    if (bind(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("bind() failed.\n");
        closesocket(mainSocket);
        return false;
    }

    if (listen(mainSocket, 2) == SOCKET_ERROR) { // Allow 2 pending connections for pairing
        printf("Error listening on socket.\n");
        return false;
    }

    return true;
}

bool Server::WaitForConnection() {
    printf("Waiting for two clients to connect...\n");

    // Wait for the first client to connect
    SOCKET client1Socket = accept(mainSocket, NULL, NULL);
    if (client1Socket == INVALID_SOCKET) {
        printf("accept() failed for client 1.\n");
        return false;
    }
    printf("Client 1 connected.\n");

    // Wait for the second client to connect
    SOCKET client2Socket = accept(mainSocket, NULL, NULL);
    if (client2Socket == INVALID_SOCKET) {
        printf("accept() failed for client 2.\n");
        closesocket(client1Socket); // Close the first client socket
        return false;
    }
    printf("Client 2 connected.\n");

    std::lock_guard<std::mutex> lock(clientsMutex);
    clientSockets.push_back(client1Socket);
    clientSockets.push_back(client2Socket);

    // Pair the clients
    clientPairs[client1Socket] = client2Socket;
    clientPairs[client2Socket] = client1Socket;

    return true;
}

void Server::HandleChat(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);

    // Check if the client is correctly paired
    if (clientPairs.find(clientSocket) == clientPairs.end()) {
        std::cout << "Client not paired correctly. Closing the connection." << std::endl;
        closesocket(clientSocket);
        return;
    }

    SOCKET pairedClient = clientPairs[clientSocket];

    while (true) {
        char message[256] = "";
        int bytesRecv = recv(clientSocket, message, sizeof(message), 0);
        if (bytesRecv <= 0) {
            std::cout << "Connection closed by the client." << std::endl;
            break;
        }

        std::cout << "Client: " << message << std::endl;

        // Forward message to the paired client
        SendToClient(pairedClient, message);
    }
}

void Server::SendToClient(SOCKET clientSocket, const char* message) {
    int bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cout << "Error sending data to client." << std::endl;
    }
}

void Server::CloseConnection() {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET clientSocket : clientSockets) {
        closesocket(clientSocket);
    }
    closesocket(mainSocket);
    WSACleanup();
}

SOCKET Server::GetClientSocket(size_t index) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    if (index < clientSockets.size()) {
        return clientSockets[index];
    }
    return INVALID_SOCKET;
}

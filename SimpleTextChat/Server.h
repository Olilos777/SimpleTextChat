#pragma once

#include <iostream>
#include <Winsock2.h>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

class Server
{
private:
    WSADATA wsaData;
    SOCKET mainSocket;
    std::unordered_map<SOCKET, SOCKET> clientPairs;
    std::vector<SOCKET> clientSockets;
    std::mutex clientsMutex;

public:
    Server();
    ~Server();

    bool Initialize();
    bool WaitForConnection();
    void HandleChat(SOCKET clientSocket);
    void SendToClient(SOCKET clientSocket, const char* message);
    void CloseConnection();

    SOCKET GetClientSocket(size_t index);
};

#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class Client {
private:
    WSADATA wsaData;
    SOCKET mainSocket;

public:
    Client();
    ~Client();

    bool Initialize();
    bool ConnectToServer();
    void SendAndReceive();
    void SendToServer(const char* message);
    void HandleChat();
    void CloseConnection();
};
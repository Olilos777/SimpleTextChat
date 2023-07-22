// SimpleTextChat.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//
#include "Server.h"

#include <iostream>
#include <string>
#include <thread>

//////////////////////
////////////////////// logowanie po uwierzytelnieniu serwer bedzie identyfikowal klientow
///////////////////////


void ClientThread(Server& server, SOCKET clientSocket) {
    server.HandleChat(clientSocket);
}

int main() {
    Server server;
    if (!server.Initialize()) {
        std::cout << "Server initialization failed." << std::endl;
        return 1;
    }

    while (true) {
        if (server.WaitForConnection()) {
            std::vector<SOCKET> clientSockets;
            clientSockets.push_back(server.GetClientSocket(0));
            clientSockets.push_back(server.GetClientSocket(1));

            std::thread clientThread1(ClientThread, std::ref(server), clientSockets[0]);
            std::thread clientThread2(ClientThread, std::ref(server), clientSockets[1]);

            clientThread1.join();
            clientThread2.join();
        }
    }

    return 0;
}

// Client.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "Client.h"

#include <iostream>
#include <string>
#include <thread>

int main()
{
    // Inicjalizacja klienta
    Client client;
    if (!client.Initialize()) {
        std::cout << "Client initialization failed." << std::endl;
        return 1;
    }

    if (!client.ConnectToServer()) {
        std::cout << "Connection to the server failed." << std::endl;
        return 1;
    }

    // Uruchomienie obsługi czatu w osobnym wątku
    std::thread chatThread([&]() {
        client.HandleChat();
        });

    // Pobieramy wiadomości od użytkownika z konsoli i wysyłamy je do serwera
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/exit") {
            break; // Wysyłamy /exit, aby wyjść z pętli
        }
        client.SendToServer(message.c_str());
    }

    // Po zakończeniu wysyłania, zamknij połączenie i poczekaj na zakończenie wątku czatu
    client.CloseConnection();
    chatThread.join();

    system("pause");
    return 0;
}

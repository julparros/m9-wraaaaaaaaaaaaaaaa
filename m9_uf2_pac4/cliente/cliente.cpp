
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9100
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Error en iniciar Winsock." << std::endl;
        return 1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Error creant el socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        std::cerr << "Error en la conversió de l'adreça IP." << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "No s'ha pogut connectar amb el servei." << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connexió establerta amb el servei." << std::endl;

    std::cin.ignore();
    std::cout << "Introduïu la vostra comanda: ";
    std::string order;
    std::getline(std::cin, order);

    send(client_socket, order.c_str(), static_cast<int>(order.size()), 0);

    char buffer[1024] = { 0 };
    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        std::cout << "Identificador rebut: " << buffer << std::endl;
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}

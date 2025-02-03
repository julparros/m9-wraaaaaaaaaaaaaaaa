
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <thread>
#include <mutex>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9100

std::mutex file_mutex;
int order_count = 1;

std::string generate_order_id() {
    std::ostringstream oss;
    oss << "ORD-" << order_count++;
    return oss.str();
}

void handle_client(SOCKET client_socket) {
    char buffer[1024] = { 0 };
    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        std::string order(buffer);

        std::lock_guard<std::mutex> lock(file_mutex);
        std::ofstream file("comandes.txt", std::ios::app);
        if (!file) {
            std::cerr << "Error obrint el fitxer!" << std::endl;
            closesocket(client_socket);
            return;
        }

        std::string order_id = generate_order_id();
        file << order_id << ": \"" << order << "\"" << std::endl;
        file.close();

        send(client_socket, order_id.c_str(), order_id.size(), 0);
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Error en iniciar Winsock." << std::endl;
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Error creant el socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error en fer el bind." << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        std::cerr << "Error en escoltar al port." << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servei iniciat. Escoltant al port " << PORT << "..." << std::endl;

    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket != INVALID_SOCKET) {
            std::cout << "Nova connexió acceptada." << std::endl;
            std::thread(handle_client, client_socket).detach();
        }
        else {
            std::cerr << "Error acceptant la connexió." << std::endl;
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

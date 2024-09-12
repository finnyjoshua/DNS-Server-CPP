#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")  // Link with Ws2_32.lib

using namespace std;

#define SERVER_PORT 12345

void startDnsClient() {
    string domainName;
    cout << "Enter the domain name: ";
    getline(cin, domainName);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(SERVER_PORT);

    sendto(clientSocket, domainName.c_str(), domainName.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    char response[1024];
    int responseLen = recvfrom(clientSocket, response, sizeof(response), 0, nullptr, nullptr);
    response[responseLen] = '\0';

    cout << "IP Address for " << domainName << ": " << response << endl;

    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    startDnsClient();
    return 0;
}

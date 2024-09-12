#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")  // Link with Ws2_32.lib

using namespace std;

#define SERVER_PORT 12345

// Function to load DNS records from a file
unordered_map<string, string> loadDnsRecords(const string& filename) {
    unordered_map<string, string> dnsRecords;
    ifstream file(filename);
    string domain, ip;
    
    while (file >> domain >> ip) {
        dnsRecords[domain] = ip;
    }
    return dnsRecords;
}

// Function to handle DNS requests and return the IP address
void handleDnsRequest(SOCKET clientSocket, sockaddr_in& clientAddr, const unordered_map<string, string>& dnsRecords) {
    char buffer[1024];
    int clientAddrLen = sizeof(clientAddr);
    
    // Receive data from the client
    int recvLen = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
    if (recvLen == SOCKET_ERROR) {
        cerr << "recvfrom failed" << endl;
        return;
    }

    buffer[recvLen] = '\0';
    string domainName(buffer);

    // Prepare response
    string response;
    auto it = dnsRecords.find(domainName);
    if (it != dnsRecords.end()) {
        response = it->second;
    } else {
        response = "Domain Not Found!";
    }

    // Send response to the client
    sendto(clientSocket, response.c_str(), response.size(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    unordered_map<string, string> dnsRecords = loadDnsRecords("dns_record.txt");

    cout << "DNS Server is running..." << endl;
    while (true) {
        sockaddr_in clientAddr;
        handleDnsRequest(serverSocket, clientAddr, dnsRecords);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

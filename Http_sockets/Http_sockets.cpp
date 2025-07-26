// Sockets in C.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <winsock2.h> // For Winsock functions and structures (e.g., SOCKET, sockaddr_in, connect, closesocket)
#include <ws2tcpip.h> // For inet_pton, etc.
#include <stdio.h>    // For printf
#include <stdlib.h>   // For exit
#include <string.h>   // For memset, strlen

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define IP "87.248.100.216" /* www.yahoo.com */
#define PORT 80

int main()
{
    // --- Winsock Initialization (REQUIRED on Windows) ---
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    // ----------------------------------------------------

    SOCKET s; // Use SOCKET type for Windows sockets
    struct sockaddr_in sock;
    // FIX 1: Change char* to const char* for 'data'
    const char* data;

    data = "HEAD / HTTP/1.0\r\n\r\n";

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // IPPROTO_TCP is the protocol for SOCK_STREAM

    if (s == INVALID_SOCKET) // Check for INVALID_SOCKET on Windows
    {
        printf("socket() error: %ld\n", WSAGetLastError()); // Get specific Winsock error code
        WSACleanup(); // Clean up Winsock
        return -1;
    }

    // FIX 2: Use inet_pton() instead of inet_addr()
    // Zero out the address structure first
    memset(&sock, 0, sizeof(sock));

    sock.sin_family = AF_INET;
    iResult = inet_pton(AF_INET, IP, &(sock.sin_addr));
    if (iResult != 1) { // inet_pton returns 1 on success, 0 if address is invalid, -1 on error
        printf("inet_pton failed or invalid IP address: %d\n", iResult);
        closesocket(s);
        WSACleanup();
        return 1;
    }

    sock.sin_port = htons(PORT);
    // sock.sin_family = AF_INET; // Already set above

    // Fix typo: (stuct sockaddr*) should be (struct sockaddr*)
    if (connect(s, (struct sockaddr*)&sock, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        printf("connect() error: %ld\n", WSAGetLastError()); // Get specific Winsock error code
        closesocket(s); // Use closesocket() on Windows
        WSACleanup();   // Clean up Winsock
        return 1;
    }

    // Use send() instead of write() for sending data over sockets on Windows
    if (send(s, data, strlen(data), 0) == SOCKET_ERROR) {
        printf("send() error: %ld\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    char buf[512]; // Declare buffer here, or where needed
    memset(buf, 0, 512);

    // Use recv() instead of read() for receiving data over sockets on Windows
    int bytes_received = recv(s, buf, 511, 0);
    if (bytes_received == SOCKET_ERROR) {
        printf("recv() error: %ld\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }
    else if (bytes_received == 0) {
        printf("Connection closed by peer.\n");
    }
    else {
        buf[bytes_received] = '\0'; // Null-terminate the received data
        printf("\n%s\n", buf); // Correct printf format specifier for string is %s
    }

    closesocket(s); // Use closesocket() on Windows

    // --- Winsock Cleanup (REQUIRED on Windows) ---
    WSACleanup();
    // -------------------------------------------

    return 0;
}
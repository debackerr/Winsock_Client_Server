#define _WIN32_WINNT 0x501

#include "stdafx.h"
#include "application.h"

#pragma comment(lib, "Ws2_32.lib")

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#undef UNICODE

void usage(int argc, char **argv) {
  printf("Missing IP address\n");
  printf("usage: %s <v4|v6> \n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        usage(argc, argv);
    }

    WSADATA wsaData;

    int iResult;

    // Initialize Winsock:
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Creating socket:
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Exchanging messages:
    int recvbuflen = DEFAULT_BUFLEN;
    const char *sendbuf = "Request message";
    char recvbuf[DEFAULT_BUFLEN];

    int a,b;
    a = 0; b = 10;
    while(1){

        // Send an initial buffer
        iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);

        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        printf("Bytes Sent: %ld\n", iResult);
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        while(a < b){
            sendbuf = "Msg here\n";
            iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);

            if (iResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes Sent: %ld\n", iResult);

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0){
                //APPLICATION RUNS HERE
                printf("Bytes received: %d\n", iResult);
                //runApp();
                puts(recvbuf);
            }
            else if (iResult == 0){
                printf("Connection closed\n");
                break;
            }
            else{
                printf("recv failed: %d\n", WSAGetLastError());
            }
            a++;
        } 

        // shutdown the send half of the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
    }
    

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;

}
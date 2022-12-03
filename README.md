# Winsock_Client_Server
Simples Winsock Client-Server made up from Microsoft's provided examples

First, compile both client and server file by writting this in terminal:

g++  -o server server.cpp -lws2_32
g++  -o client client.cpp -lws2_32

To execute files, use 2 different terminals and provide an IP address as it follows:

server.exe 127.0.0.1
client.exe 127.0.0.1





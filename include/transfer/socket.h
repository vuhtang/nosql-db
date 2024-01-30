#ifndef LLP_SOCKET_H
#define LLP_SOCKET_H

#include "arpa/inet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 40096
#define SERVER_PORT 34908 // same as in client
#define MAX_CONNECTIONS 5

int socket_server();
void bind_socket(int server_socket);
int socket_accept(int server_socket);
void receive_from_client(int client_socket, char *buffer);
void send_to_client(int client_socket, char *buffer);
void close_socket(int socket);

#endif//LLP_SOCKET_H

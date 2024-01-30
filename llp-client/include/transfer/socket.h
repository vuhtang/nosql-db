#ifndef LLP_CLIENT_SOCKET_H
#define LLP_CLIENT_SOCKET_H

#include "arpa/inet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 40096
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 34908 // same as in server

int socket_client();
void connect_to_server(int client_socket, char *server_ip, int server_port);
void send_to_server(int client_socket, char *buffer);
void receive_from_server(int client_socket, char *buffer);
void close_socket(int socket);

#endif//LLP_CLIENT_SOCKET_H

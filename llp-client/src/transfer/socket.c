#include "transfer/socket.h"

int socket_client() {
    int client = socket(AF_INET, SOCK_STREAM, 0);
    return client;
}

void connect_to_server(int client_socket, char *server_ip, int server_port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));
    connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
}

void send_to_server(int client_socket, char *buffer) {
    send(client_socket, buffer, strlen(buffer), 0);
}

void receive_from_server(int client_socket, char *buffer) {
    memset(buffer, 0, MAX_BUFFER_SIZE);
    recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
}

void close_socket(int socket) {
    close(socket);
}
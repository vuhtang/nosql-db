#include "transfer/socket.h"

int socket_server() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

void bind_socket(int server_socket) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

int socket_accept(int server_socket) {
    return accept(server_socket, NULL, NULL);
}

void receive_from_client(int client_socket, char *buffer) {
    ssize_t bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("Client disconnected.\n");
        } else {
            perror("recv");
        }
    } else {
        buffer[bytes_received] = '\0';
    }
}

void send_to_client(int client_socket, char *buffer) {
    send(client_socket, buffer, strlen(buffer), 0);
}

void close_socket(int socket) {
    close(socket);
}
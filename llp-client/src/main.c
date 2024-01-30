#include <stdio.h>
#include "client/input.h"
#include "transfer/socket.h"

int main() {

    int client = socket_client();
    connect_to_server(client, SERVER_IP, SERVER_PORT);

    int times = 20;
    while(times) {
        times--;
        char buffer[MAX_BUFFER_SIZE];
        input();
        char *xml = to_xml();
        strcpy(buffer, xml);
        free_ast_node(get_root_ast_node());
        printf("%s", buffer);
        send_to_server(client, buffer);
        receive_from_server(client, buffer);
        printf("%s", buffer);
    }

    close_socket(client);

    return 0;
}

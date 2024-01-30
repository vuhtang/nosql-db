#include "db/tests/tests.h"
#include "serializer/from_xml.h"
#include "transfer/socket.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd = open("file.bin", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        printf("couldn't get file size\n");
        return -1;
    }

    struct file *f;

    if (sb.st_size == 0) {
        f = init_file(fd);
        printf("file created\n");
    } else {
        f = read_file(fd);
        printf("file was read\n");
    }

    int server = socket_server();
    bind_socket(server);
    listen(server, MAX_CONNECTIONS);

    int client = socket_accept(server);

    int times = 20;

    while (times) {
        times--;
        char buffer[MAX_BUFFER_SIZE];
        receive_from_client(client, buffer);
        printf("%s\n", buffer);
        handle_xml_and_return_response_at_buffer(buffer, f);
        send_to_client(client, buffer);
    }


    close_socket(server);
    close_socket(client);
    del_file(f);
    return 0;
}
#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/param.h>
#include <net.h>

#define ADDRESS "127.0.0.1"
#define PORT 3000

void handle_connection(int connection) {
        unsigned int buff_size = 0;
        char *header = malloc(buff_size);

        read_header(connection, &header, buff_size);

        printf("%s", header);

        unsigned int content_length = strlen(header);

        char response[1024];

        snprintf(response, 1024, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\n%s", content_length, header);

        send(connection, response, sizeof(response), 0);

        free(header);

        shutdown(connection, SHUT_RDWR);
        close(connection);
}

int main() {
    int socket_fd;
    const unsigned int FAMILY = AF_INET;

    // create a TCP socket and obtain fd for said socket
    if ((socket_fd = socket(FAMILY, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = FAMILY;
    // convert and set binding port
    addr.sin_port = htons(PORT);
    // convert and set binding address
    inet_pton(FAMILY, ADDRESS, &addr.sin_addr);

    // resuse port
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // bind to ADDRESS:PORT
    if ((bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("Failed to bind to ADDRESS:PORT");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(socket_fd, 1) < 0) {
        perror("Failed to listen to address");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int connection;
        socklen_t addr_len = sizeof(addr);
        
        // accept incoming connection, return new socket fd for this client
        if ((connection = accept(socket_fd, (struct sockaddr *)&addr, &addr_len)) < 0) {
            perror("Couldn't accept connections");
            exit(EXIT_FAILURE);
        }

        puts("-> Accepted connection");

        // Handle connection
        handle_connection(connection);

        puts("-> Closing connection");
    }

    return 0;
}

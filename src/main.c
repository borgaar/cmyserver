#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 1024
#define ADDRESS "127.0.0.1"
#define PORT 3000

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

    // bind to ADDRESS:PORT
    if ((bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("Failed to bind to ADDRESS:PORT");
        exit(EXIT_FAILURE);
    }

    // resuse port
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
            perror("Can't accept connections");
            exit(EXIT_FAILURE);
        }

        puts("-> Accepted connection");

        int bytes_read = 0;
        char buff[BUFF_SIZE];

        while (1) {
            // wipe buffer
            memset(buff, '\0', BUFF_SIZE);

            // read incoming bytes
            if ((bytes_read = recv(connection, buff, BUFF_SIZE - 1, 0)) < 0) {
                perror("Failed to read incoming data");
                exit(EXIT_FAILURE);
            }

            if (bytes_read == 0 || (buff[bytes_read - 1] == '\n' && bytes_read <= 2)) {
                break;
            }

            printf("%s", buff);
        }

        puts("-> Closing connection");

        shutdown(connection, SHUT_RDWR);
        close(connection);
    }

    return 0;
}

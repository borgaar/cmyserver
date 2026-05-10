#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>

#define STEP_SIZE 1

int read_header(unsigned int connection, char **buff, unsigned int buff_size) {
        unsigned int total_read = 0;
        int read_bytes;
        char new_read[STEP_SIZE + 1];
        memset(*buff, '\0', buff_size);

        while (1) {
            memset(new_read, '\0', sizeof(new_read));

            // read incoming bytes
            if ((read_bytes = recv(connection, new_read, STEP_SIZE, 0)) < 0) {
                perror("Failed to read incoming data");
                return -1;
            }

            // resize header buffer if it is full
            if (total_read + read_bytes >= buff_size) {
                buff_size = MAX(buff_size, 1) * 2;
                char *tmp = realloc(*buff, buff_size);

                if (!tmp) {
                    perror("realloc failed");
                    return -1;
                }

                *buff = tmp;
            };

            (*buff)[total_read] = '\0';

            strcat(*buff, new_read);

            total_read += read_bytes;

            unsigned int header_len = strlen(*buff);

            if (
                    header_len >= 4 &&
                    (*buff)[header_len - 1] == '\n' &&
                    (*buff)[header_len - 2] == '\r' &&
                    (*buff)[header_len - 3] == '\n' &&
                    (*buff)[header_len - 4] == '\r'
                    ) {
                return header_len;
            }
        }
    return -1;
}


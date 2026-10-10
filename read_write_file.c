
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main(void)
{
    int fd;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE];

    fd = open("data.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    printf("Original file contents:\n%s\n", buffer);

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return EXIT_FAILURE;
    }

    if (bytes_read >= 5) {
        buffer[0] = 'H';
        buffer[1] = 'E';
        buffer[2] = 'L';
        buffer[3] = 'L';
        buffer[4] = 'O';
    }

    ssize_t total_written = 0;

    while (total_written < bytes_read) {
        ssize_t n = write(fd, buffer + total_written,
                          bytes_read - total_written);

        if (n <= 0) {
            perror("write");
            close(fd);
            return EXIT_FAILURE;
        }

        total_written += n;
    }

    printf("File modified using read()/write().\n");

    close(fd);
    return EXIT_SUCCESS;
}

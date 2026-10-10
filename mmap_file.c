
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main(void)
{
    int fd = open("data.txt", O_RDWR);
    struct stat st;
    char *data;

    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    if (st.st_size == 0) {
        printf("File is empty.\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Original file contents:\n");
    fwrite(data, 1, st.st_size, stdout);

    if (st.st_size >= 5)
        memcpy(data, "HELLO", 5);

    if (msync(data, st.st_size, MS_SYNC) == -1)
        perror("msync");
    else
        printf("\nFile modified using mmap().\n");

    munmap(data, st.st_size);
    close(fd);
    return EXIT_SUCCESS;
}

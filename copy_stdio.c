#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 8192

int main(int argc, char *argv[])
{
    FILE *src;
    FILE *dest;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    /* Open source file */
    src = fopen(argv[1], "rb");
    if (src == NULL)
    {
        perror("fopen source");
        return 1;
    }

    /* Open destination file */
    dest = fopen(argv[2], "wb");
    if (dest == NULL)
    {
        perror("fopen destination");
        fclose(src);
        return 1;
    }

    /* Copy file using fread() and fwrite() */
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0)
    {
        size_t total_written = 0;

        while (total_written < bytes_read)
        {
            size_t bytes_written = fwrite(
                buffer + total_written,
                1,
                bytes_read - total_written,
                dest
            );

            if (bytes_written == 0)
            {
                fprintf(stderr, "Error writing destination file\n");
                fclose(src);
                fclose(dest);
                return 1;
            }

            total_written += bytes_written;
        }
    }

    if (ferror(src))
    {
        fprintf(stderr, "Error reading source file\n");
        fclose(src);
        fclose(dest);
        return 1;
    }

    /* Determine source file size using fseek() and ftell() */
    if (fseek(src, 0, SEEK_END) == 0)
    {
        long file_size = ftell(src);

        if (file_size >= 0)
            printf("Source file size: %ld bytes\n", file_size);
    }

    if (fclose(src) != 0)
    {
        perror("fclose source");
        fclose(dest);
        return 1;
    }

    if (fclose(dest) != 0)
    {
        perror("fclose destination");
        return 1;
    }

    printf("File copied successfully.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        const char *usage = "Usage: ./my_copy <source> <dest>\n";
        write(STDERR_FILENO, usage, strlen(usage));
        return 1;
    }

    if (access(argv[2], F_OK) == 0) {
        char response;
        const char *warn = "Target file already exists. Overwrite? (y/n): ";
        while (1) {
            write(STDOUT_FILENO, warn, strlen(warn));
            if (read(STDIN_FILENO, &response, 1) <= 0) return 1;

            char temp;
            while (read(STDIN_FILENO, &temp, 1) > 0 && temp != '\n');

            if (response == 'y' || response == 'Y') {
                break;
            } else if (response == 'n' || response == 'N') {
                const char *cancel = "Copy canceled by user.\n";
                write(STDOUT_FILENO, cancel, strlen(cancel));
                return 0;
            }
        }
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        return 1;
    }

    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error opening destination file");
        close(src_fd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t n_read, n_written;

    while ((n_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        n_written = write(dest_fd, buffer, n_read);
        if (n_written != n_read) {
            perror("Error during writing");
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    if (n_read < 0) {
        perror("Error during reading");
    }

    close(src_fd);
    close(dest_fd);

    const char *success = "File copied successfully.\n";
    write(STDOUT_FILENO, success, strlen(success));

    return 0;
}


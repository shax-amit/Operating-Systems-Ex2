#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        const char *usage = "Usage: ./my_copy <source> <dest>\n";
        write(STDERR_FILENO, usage, 33);
        return 1;
    }

    if (access(argv[2], F_OK) == 0) {
        char response;
        const char *warn_msg = "Target file already exists. Overwrite? (y/n): ";
        while (1) {
            write(STDOUT_FILENO, warn_msg, 47);

            if (read(STDIN_FILENO, &response, 1) <= 0) return 1;

            if (response != '\n') {
                char dummy;
                while (read(STDIN_FILENO, &dummy, 1) > 0 && dummy != '\n');
            } else {
                continue;
            }

            if (response == 'y' || response == 'Y') {
                break;
            } else if (response == 'n' || response == 'N') {
                const char *cancel_msg = "The copy was canceled at the user's request.\n";
                write(STDOUT_FILENO, cancel_msg, 46);
                return 0;
            }
        }
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        const char *err_src = "Error: Cannot open source file\n";
        write(STDERR_FILENO, err_src, 31);
        return 1;
    }

    int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        const char *err_dest = "Error: Cannot open destination file\n";
        write(STDERR_FILENO, err_dest, 36);
        close(src_fd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t n_read, n_written;

    while ((n_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        n_written = write(dest_fd, buffer, n_read);
        if (n_written != n_read) {
            const char *err_write = "Error: Failed to write to destination\n";
            write(STDERR_FILENO, err_write, 38);
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    if (n_read < 0) {
        const char *err_read = "Error: Failed to read from source\n";
        write(STDERR_FILENO, err_read, 34);
    }

    close(src_fd);
    close(dest_fd);

    const char *success = "File copied successfully.\n";
    write(STDOUT_FILENO, success, 26);

    return 0;
}

#include "../src/http_client.h"
#include "../src/socket_io.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define HOST "localhost"
#define PORT "80"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,
            "usage: %s \"<request-line>\"\n"
            "example: %s \"GET /calc?query=2+10 HTTP/1.1\"\n",
            argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    const char *request_line = argv[1];

    const int fd = open_client_fd(HOST, PORT);
    if (fd == -1) {
        const int err = errno;
        fprintf(stderr, "open_client_fd: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    char *request = NULL;
    size_t request_length = 0;
    if (build_request_dup(&request, &request_length,
                          request_line) < 0) {
        const int err = errno;
        fprintf(stderr, "build_request_dup: %s\n", strerror(err));
        close(fd);
        return EXIT_FAILURE;
    }

    if (send_all(fd, request, request_length) < 0) {
        const int err = errno;
        fprintf(stderr, "send_all: %s\n", strerror(err));
        free(request);
        close(fd);
        return EXIT_FAILURE;
    }
    free(request);

    shutdown(fd, SHUT_WR);

    char *response = NULL;
    const ssize_t bytes_read = read_all(fd, &response);
    if (bytes_read < 0) {
        const int err = errno;
        fprintf(stderr, "read_all: %s\n", strerror(err));
        close(fd);
        return EXIT_FAILURE;
    }

    fwrite(response, 1, (size_t)bytes_read, stdout);
    free(response);

    close(fd);
    return EXIT_SUCCESS;
}

#include "../src/http_calc.h"
#include "../src/http_request.h"
#include "../src/http_request_io.h"
#include "../src/http_response.h"
#include "../src/socket_io.h"
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 80
#define BACKLOG 128

static volatile sig_atomic_t stop_flag = 0;

void handler(int sig) {
  stop_flag = 1;
}

static void handle_connection(int client_fd) {
  char *request_line_string = NULL;
  char *calculation_result = NULL;
  char *response = NULL;
  RequestLine request_line = {0};

  if (read_request_line(client_fd, &request_line_string) != 0)
    goto cleanup;
  if (parse_request_line_dup(request_line_string, &request_line) != 0)
    goto cleanup;
  if (calc_eval_request_line_dup(&request_line, &calculation_result) != 0)
    goto cleanup;
  if (build_resp_dup(&response, calculation_result) != 0)
    goto cleanup;

  if (send_all(client_fd, response, strlen(response)) != 0) {
    int error = errno;
    if (error == EPIPE || error == ECONNRESET) {
      fprintf(stderr, "Connection closed by peer\n");
    } else {
      fprintf(stderr, "Connection system error: %s\n", strerror(error));
    }
  }

cleanup:
  request_line_cleanup(&request_line);
  free(response);
  free(calculation_result);
  free(request_line_string);
  close(client_fd);
}


int main(void) {
  struct sigaction sa = {0};
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  const int server_socket = socket(AF_INET6, SOCK_STREAM, 0);
  if (server_socket == -1) {
    fprintf(stderr, "[socket] errno=%d (%s)\n", errno, strerror(errno));
    return EXIT_FAILURE;
  }

  const int enable_reuse = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable_reuse,
                 sizeof(enable_reuse)) == -1) {
    fprintf(stderr, "[setsockopt] errno=%d (%s)\n", errno, strerror(errno));
    close(server_socket);
    return EXIT_FAILURE;
  }

  const int ipv6only = 0;
  if (setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY,
               &ipv6only, sizeof(ipv6only)) == -1) {
    fprintf(stderr, "[setsockopt IPV6_V6ONLY] errno=%d (%s)\n",
            errno, strerror(errno));
    close(server_socket);
    return EXIT_FAILURE;
  }

  struct sockaddr_in6 server_address = {0};
  server_address.sin6_family = AF_INET6;
  server_address.sin6_port = htons(PORT);
  server_address.sin6_addr = in6addr_any;

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    fprintf(stderr, "[bind] errno=%d (%s)\n", errno, strerror(errno));
    close(server_socket);
    return EXIT_FAILURE;
  }
  if (listen(server_socket, BACKLOG) == -1) {
    fprintf(stderr, "[listen] errno=%d (%s)\n", errno, strerror(errno));
    close(server_socket);
    return EXIT_FAILURE;
  }

  for (;;) {
    const int client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
      const int error = errno;
      if (error == EINTR) {
        if (stop_flag) break;
        continue;
      }
      if (error == ECONNABORTED)
        continue;
      if (error == EMFILE || error == ENFILE || error == ENOMEM) {
        usleep(10000);
        continue;
      }
      fprintf(stderr, "[accept] errno=%d (%s)\n", error, strerror(error));
      continue;
    }
    handle_connection(client_fd);

    if (stop_flag) {
      break;
    }
  }
  close(server_socket);
}

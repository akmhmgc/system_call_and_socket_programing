#include "../src/http_calc.h"
#include "../src/http_request.h"
#include "../src/http_response.h"
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 80
#define BACKLOG 128

typedef enum {
  SEND_OK = 0,
  SEND_PEER_CLOSED,
  SEND_SYSERR,
} send_result_t;

static int send_all(int fd, const void *buf, size_t len) {
  const char *start = buf;
  size_t left = len;
  while (left > 0) {
    ssize_t sent_len = send(fd, start, left, MSG_NOSIGNAL);
    if (sent_len > 0) {
      start += (size_t)sent_len;
      left -= (size_t)sent_len;
      continue;
    }
    if (sent_len == 0)
      return SEND_PEER_CLOSED;
    if (errno == EPIPE || errno == ECONNRESET)
      return SEND_PEER_CLOSED;
    if (errno == EINTR)
      continue;
    return SEND_SYSERR;
  }
  return SEND_OK;
}

static void handle_connection(int fd) {
  char *request_line_char = NULL;
  char *ans = NULL;
  char *response = NULL;
  RequestLine request_line = {0};

  if (read_request_line(fd, &request_line_char) != 0)
    goto out;
  if (parse_request_line_dup(request_line_char, &request_line) != 0)
    goto out;
  if (calc_eval_request_line_dup(&request_line, &ans) != 0)
    goto out;
  if (build_resp_dup(&response, ans) != 0)
    goto out;

  if (send_all(fd, response, strlen(response)) != 0) {
    int err = errno;
    switch (err) {
    case SEND_PEER_CLOSED:
      fprintf(stderr, "Connection closed by peer\n");
      break;
    case SEND_SYSERR:
      fprintf(stderr, "Connection system error\n");
      break;
    default:
      break;
    }
  }

out:
  request_line_cleanup(&request_line);
  free(response);
  free(ans);
  free(request_line_char);
  close(fd);
}

int main(void) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    fprintf(stderr, "[socket] errno=%d (%s)\n", errno, strerror(errno));
    return 1;
  }

  int yes = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    fprintf(stderr, "[setsockopt] errno=%d (%s)\n", errno, strerror(errno));
    close(sock);
    return 1;
  }

  struct sockaddr_in sa = {0};
  sa.sin_family = AF_INET;
  sa.sin_port = htons(PORT);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
    fprintf(stderr, "[bind] errno=%d (%s)\n", errno, strerror(errno));
    close(sock);
    return 1;
  }
  if (listen(sock, BACKLOG) == -1) {
    fprintf(stderr, "[listen] errno=%d (%s)\n", errno, strerror(errno));
    close(sock);
    return 1;
  }

  for (;;) {
    int fd = accept(sock, NULL, NULL);
    if (fd == -1) {
      int err = errno;
      if (err == EINTR || err == ECONNABORTED)
        continue;
      if (err == EMFILE || err == ENFILE || err == ENOMEM) {
        usleep(10000);
        continue;
      }
      fprintf(stderr, "[accept] errno=%d (%s)\n", err, strerror(err));
      continue;
    }
    handle_connection(fd);
  }
}

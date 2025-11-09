#include "http_client.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>

int open_client_fd(const char *host, const char *port) {
  struct addrinfo hints, *addrinfo_list = NULL, *addr = NULL;
  int socket_fd = -1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  int getaddrinfo_result = getaddrinfo(host, port, &hints, &addrinfo_list);
  if (getaddrinfo_result != 0) {
    return -1;
  }

  for (addr = addrinfo_list; addr != NULL;
       addr = addr->ai_next) {
    socket_fd = socket(addr->ai_family, addr->ai_socktype,
                       addr->ai_protocol);
    if (socket_fd < 0)
      continue;
    if (connect(socket_fd, addr->ai_addr, addr->ai_addrlen) ==
        0)
      break;
    close(socket_fd);
    socket_fd = -1;
  }
  freeaddrinfo(addrinfo_list);
  return socket_fd;
}

int build_request_dup(char **out, size_t *out_len,
                      const char *request_line) {
  if (!request_line || !out || !out_len) { errno = EINVAL; return -1; }

  const size_t line_len = strlen(request_line);
  // +2 = "\r\n", +1 = NUL
  if (line_len > SIZE_MAX - 3) { errno = EOVERFLOW; return -1; }

  const size_t need = line_len + 2 + 1;
  char *buf = (char *)malloc(need);
  if (!buf) return -1;

  int written_buffer_size = snprintf(buf, need, "%s\r\n", request_line);
  if (written_buffer_size < 0 || (size_t)written_buffer_size >= need) { free(buf); errno = EOVERFLOW; return -1; }

  *out = buf;
  *out_len = (size_t)written_buffer_size;  // NULは含まない
  return 0;
}


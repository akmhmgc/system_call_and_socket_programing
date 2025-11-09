#include "http_response.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP_RESPONSE_FORMAT                                                   \
  "HTTP/1.1 200 OK\r\n"                                                        \
  "Content-Length: %zu\r\n"                                                    \
  "\r\n%s"

int build_resp_dup(char **out, const char *body) {
  if (!out || !body) {
    errno = EINVAL;
    return -1;
  }
  *out = NULL;

  int required_size =
      snprintf(NULL, 0, HTTP_RESPONSE_FORMAT, strlen(body), body);
  if (required_size < 0)
    return -1;

  char *buffer = malloc((size_t)required_size + 1);
  if (!buffer)
    return -1;

  int bytes_written = snprintf(buffer, (size_t)required_size + 1,
                               HTTP_RESPONSE_FORMAT, strlen(body), body);
  if (bytes_written < 0) {
    free(buffer);
    return -1;
  }

  *out = buffer;
  return 0;
}

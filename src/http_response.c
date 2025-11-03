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

  int need = snprintf(NULL, 0, HTTP_RESPONSE_FORMAT, strlen(body), body);
  if (need < 0)
    return -1;

  char *buf = malloc((size_t)need + 1);
  if (!buf)
    return -1;

  int n =
      snprintf(buf, (size_t)need + 1, HTTP_RESPONSE_FORMAT, strlen(body), body);
  if (n < 0) {
    free(buf);
    return -1;
  }

  *out = buf;
  return 0;
}

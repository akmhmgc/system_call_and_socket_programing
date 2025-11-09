#include "http_calc.h"
#include "calc.h"
#include "http_request.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int validate_request_line(const RequestLine *request_line) {
  if (!request_line) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(request_line->path, "/calc") != 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

int calc_eval_request_line_dup(const RequestLine *request_line, char **out) {
  if (!request_line || !out) {
    errno = EINVAL;
    return -1;
  }
  *out = NULL;

  if (validate_request_line(request_line) == -1) {
    errno = EINVAL;
    return -1;
  }

  char *query_value = NULL;
  if (extract_query_value_dup(request_line->query, &query_value) == -1)
    return -1;

  int ans = 0;
  const int status = eval_sum(query_value, &ans);
  free(query_value);
  if (status == -1)
    return -1;

  int need = snprintf(NULL, 0, "%d", ans);
  if (need < 0)
    return -1;

  char *buf = malloc((size_t)need + 1);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  if (snprintf(buf, (size_t)need + 1, "%d", ans) < 0) {
    free(buf);
    return -1;
  }

  *out = buf;
  return 0;
}

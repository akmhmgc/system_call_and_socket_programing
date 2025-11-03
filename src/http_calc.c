#include "http_request.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int eval_sum(const char *expression, int *out) {
  if (!expression) {
    errno = EINVAL;
    return -1;
  }

  char *plus = strchr(expression, '+');
  if (!plus) {
    errno = EINVAL;
    return -1;
  }

  errno = 0;
  char *end1 = NULL;
  const long left = strtol(expression, &end1, 10);
  if (end1 != plus) {
    errno = EINVAL;
    return -1;
  }
  if (errno == ERANGE)
    return -1;

  errno = 0;
  char *end2 = NULL;
  const long right = strtol(plus + 1, &end2, 10);
  if (end2 == plus + 1) {
    errno = EINVAL;
    return -1;
  }
  if (*end2 != '\0') {
    errno = EINVAL;
    return -1;
  }
  if (errno == ERANGE)
    return -1;

  const long sum = left + right;
  if (sum < INT_MIN || sum > INT_MAX) {
    errno = ERANGE;
    return -1;
  }

  *out = (int)sum;
  return 0;
}

int extract_query_value_dup(const char *query_string, char **out) {
  if (!query_string || !out) {
    errno = EINVAL;
    return -1;
  }
  const char *prefix = "query=";
  const size_t prefix_len = strlen(prefix);

  if (strncmp(query_string, prefix, prefix_len) != 0) {
    errno = EINVAL;
    return -1;
  }

  const char *query = query_string + prefix_len;
  const size_t query_len = strlen(query);

  char *buf = malloc(query_len + 1);
  if (!buf) {
    errno = ENOMEM;
    return -1;
  }

  memcpy(buf, query, query_len + 1);
  *out = buf;
  return 0;
}

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

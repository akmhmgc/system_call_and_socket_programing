#include "calc.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int eval_sum(const char *expression, int *out) {
  if (!expression || !out) {
    errno = EINVAL;
    return -1;
  }

  char *plus_sign = strchr(expression, '+');
  if (!plus_sign) {
    errno = EINVAL;
    return -1;
  }

  errno = 0;
  char *left_end_ptr = NULL;
  const long left_operand = strtol(expression, &left_end_ptr, 10);
  if (left_end_ptr != plus_sign) {
    errno = EINVAL;
    return -1;
  }
  if (errno == ERANGE)
    return -1;

  errno = 0;
  char *right_end_ptr = NULL;
  const long right_operand = strtol(plus_sign + 1, &right_end_ptr, 10);
  if (right_end_ptr == plus_sign + 1) {
    errno = EINVAL;
    return -1;
  }
  if (*right_end_ptr != '\0') {
    errno = EINVAL;
    return -1;
  }
  if (errno == ERANGE)
    return -1;

  const long sum = left_operand + right_operand;
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
  const char *expected_prefix = "query=";
  const size_t prefix_length = strlen(expected_prefix);

  if (strncmp(query_string, expected_prefix, prefix_length) != 0) {
    errno = EINVAL;
    return -1;
  }

  const char *query_value = query_string + prefix_length;
  const size_t query_value_length = strlen(query_value);

  char *buffer = malloc(query_value_length + 1);
  if (!buffer) {
    errno = ENOMEM;
    return -1;
  }

  memcpy(buffer, query_value, query_value_length + 1);
  *out = buffer;
  return 0;
}

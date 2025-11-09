#include "http_request_io.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static char *find_start_line_end(const char *buf, const size_t len) {
  for (size_t i = 0; i + 1 < len; i++) {
    if (buf[i] == '\r' && memcmp(buf + i, "\r\n", 2) == 0) {
      return (char *)(buf + i);
    }
  }
  return NULL;
}

#define INITIAL_CAPACITY 4096
#define MAX_REQUEST_LINE_SIZE (1024 * 1024)

int read_request_line(const int fd, char **out_line) {
  if (!out_line)
    return -1;
  *out_line = NULL;

  size_t buffer_capacity = INITIAL_CAPACITY;
  size_t bytes_read = 0;
  char *buffer = malloc(buffer_capacity);
  if (!buffer)
    return -1;

  const char *line_end = NULL;
  for (;;) {
    if (bytes_read == buffer_capacity) {
      if (buffer_capacity > SIZE_MAX / 2 ||
          buffer_capacity * 2 > MAX_REQUEST_LINE_SIZE) {
        free(buffer);
        errno = ENOMEM;
        return -1;
      }
      size_t new_capacity = buffer_capacity * 2;
      char *temp_buffer = realloc(buffer, new_capacity);
      if (!temp_buffer) {
        free(buffer);
        errno = ENOMEM;
        return -1;
      }
      buffer = temp_buffer;
      buffer_capacity = new_capacity;
    }

    ssize_t received_bytes =
        recv(fd, buffer + bytes_read, buffer_capacity - bytes_read, 0);
    if (received_bytes == 0) {
      free(buffer);
      errno = ECONNRESET;
      return -1;
    }
    if (received_bytes < 0) {
      int saved_errno = errno;
      if (saved_errno == EINTR) {
        continue;
      }
      free(buffer);
      errno = saved_errno;
      return -1;
    }
    bytes_read += (size_t)received_bytes;

    line_end = find_start_line_end(buffer, bytes_read);
    if (line_end)
      break;
  }

  size_t line_length = (size_t)(line_end - buffer);
  char *output = malloc(line_length + 1);
  if (!output) {
    free(buffer);
    return -1;
  }

  size_t write_index = 0;
  for (size_t read_index = 0; read_index < line_length; ++read_index) {
    if (buffer[read_index] == '\r')
      continue;
    output[write_index++] = buffer[read_index];
  }
  output[write_index] = '\0';

  free(buffer);
  *out_line = output;
  return 0;
}

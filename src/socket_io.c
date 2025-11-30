#include "socket_io.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>

#define READ_INITIAL_CAPACITY 1024

int send_all(int fd, const void *buf, size_t len) {
  const char *position = (const char *)buf;
  size_t bytes_remaining = len;
  while (bytes_remaining > 0) {
    ssize_t bytes_sent = send(fd, position, bytes_remaining, MSG_NOSIGNAL);
    if (bytes_sent > 0) {
      position += (size_t)bytes_sent;
      bytes_remaining -= (size_t)bytes_sent;
      continue;
    }
    if (bytes_sent == 0) {
      errno = EPIPE;
      return -1;
    }
    if (errno == EPIPE || errno == ECONNRESET)
      return -1;
    if (errno == EINTR)
      continue;
    return -1;
  }
  return 0;
}

static size_t grow_capacity(size_t capacity) {
  if (capacity > SIZE_MAX / 2) return SIZE_MAX;
  return capacity * 2;
}

ssize_t read_all(int fd, char **buf) {
  size_t capacity = READ_INITIAL_CAPACITY;
  if (*buf == NULL) {
    char *temp_buffer = (char *)malloc(capacity);
    if (!temp_buffer)
      return -1;
    *buf = temp_buffer;
  }

  size_t total_bytes_read = 0;
  for (;;) {
    if (total_bytes_read == capacity) {
      const size_t new_capacity = grow_capacity(capacity);
      char *temp_buffer = (char *)realloc(*buf, new_capacity);
      if (!temp_buffer)
        return -1;
      *buf = temp_buffer;
      capacity= new_capacity;
    }
    const ssize_t bytes_read = read(fd, *buf + total_bytes_read, capacity - total_bytes_read);
    if (bytes_read < 0) {
      if (errno == EINTR)
        continue;
      return -1;
    }
    if (bytes_read == 0)
      break; // EOF
    total_bytes_read += (size_t)bytes_read;
  }

  // NUL 終端 1byte を確保
  if (total_bytes_read == capacity) {
    char *temp_buffer = (char *)realloc(*buf, capacity + 1);
    if (!temp_buffer)
      return -1;
    *buf = temp_buffer;
    capacity += 1;
  }
  (*buf)[total_bytes_read] = '\0';
  return (ssize_t)total_bytes_read;
}

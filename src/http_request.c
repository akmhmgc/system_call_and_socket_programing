#include "http_request.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void request_line_cleanup(RequestLine *request_line) {
  if (!request_line)
    return;
  free(request_line->path);
  free(request_line->query);
  request_line->path = NULL;
  request_line->query = NULL;
}

int parse_request_line_dup(const char *line, RequestLine *out) {
  if (!line || !out) {
    errno = EINVAL;
    return -1;
  }
  request_line_cleanup(out);

  // method SP request-target SP HTTP-version
  // ref: https://www.rfc-editor.org/rfc/rfc9112.html#name-request-line
  const char *space1 = strchr(line, ' ');
  if (!space1) {
    errno = EINVAL;
    return -1;
  }
  const char *space2 = strchr(space1 + 1, ' ');
  if (!space2) {
    errno = EINVAL;
    return -1;
  }

  // method
  size_t method_len = (size_t)(space1 - line);
  if (method_len == 0 || method_len >= METHOD_LEN) {
    errno = ERANGE;
    return -1;
  }
  memcpy(out->method, line, method_len);
  out->method[method_len] = '\0';

  // target
  const char *target = space1 + 1;
  size_t target_len = (size_t)(space2 - target);
  if (target_len == 0) {
    errno = EINVAL;
    return -1;
  }

  // path と query に分割
  const char *question_mark = memchr(target, '?', target_len);
  if (question_mark) {
    size_t path_len = (size_t)(question_mark - target);
    if (path_len == 0) {
      errno = EINVAL;
      return -1;
    }

    out->path = malloc(path_len + 1);
    if (!out->path) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(out->path, target, path_len);
    out->path[path_len] = '\0';

    size_t query_len = (size_t)(target_len - (path_len + 1));
    if (query_len > 0) {
      out->query = malloc(query_len + 1);
      if (!out->query) {
        free(out->path);
        out->path = NULL;
        errno = ENOMEM;
        return -1;
      }
      memcpy(out->query, question_mark + 1, query_len);
      out->query[query_len] = '\0';
    } else {
      out->query = malloc(1);
      if (!out->query) {
        free(out->path);
        out->path = NULL;
        errno = ENOMEM;
        return -1;
      }
      out->query[0] = '\0';
    }
  } else {
    out->path = malloc(target_len + 1);
    if (!out->path) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(out->path, target, target_len);
    out->path[target_len] = '\0';

    out->query = malloc(1);
    if (!out->query) {
      free(out->path);
      out->path = NULL;
      errno = ENOMEM;
      return -1;
    }
    out->query[0] = '\0';
  }

  // version
  const char *version = space2 + 1;
  size_t version_len = strlen(version);
  if (version_len == 0 || version_len >= VERSION_LEN) {
    request_line_cleanup(out);
    errno = ERANGE;
    return -1;
  }
  memcpy(out->version, version, version_len);
  out->version[version_len] = '\0';

  return 0;
}

int validate_request_common(const RequestLine *request_line) {
  if (!request_line) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(request_line->method, "GET") != 0) {
    errno = EINVAL;
    return -1;
  }
  if (strcmp(request_line->version, "HTTP/1.1") != 0) {
    errno = EINVAL;
    return -1;
  }
  return 0;
}

static char *find_start_line_end(const char *buf, const size_t len) {
  for (size_t i = 0; i + 1 < len; i++) {
    if (buf[i] == '\r' && memcmp(buf + i, "\r\n", 2) == 0) {
      return (char *)(buf + i);
    }
  }
  return NULL;
}

#define INIT_CAP 4096
#define MAX_REQUEST_LINE_SIZE (1024 * 1024)

int read_request_line(const int fd, char **out_line) {
  if (!out_line)
    return -1;
  *out_line = NULL;

  size_t cap = INIT_CAP;
  size_t len = 0;
  char *buf = malloc(cap);
  if (!buf)
    return -1;

  const char *line_end = NULL;
  for (;;) {
    if (len == cap) {
      if (cap > SIZE_MAX / 2 || cap * 2 > MAX_REQUEST_LINE_SIZE) {
        free(buf);
        errno = ENOMEM;
        return -1;
      }
      size_t newcap = cap * 2;
      char *tmp = realloc(buf, newcap);
      if (!tmp) {
        free(buf);
        errno = ENOMEM;
        return -1;
      }
      buf = tmp;
      cap = newcap;
    }

    ssize_t n = recv(fd, buf + len, cap - len, 0);
    if (n == 0) {
      free(buf);
      errno = ECONNRESET;
      return -1;
    }
    if (n < 0) {
      int saved_errno = errno;
      if (saved_errno == EINTR) {
        continue;
      }
      free(buf);
      errno = saved_errno;
      return -1;
    }
    len += (size_t)n;

    line_end = find_start_line_end(buf, len);
    if (line_end)
      break;
  }

  size_t line_len = (size_t)(line_end - buf);
  char *out = malloc(line_len + 1);
  if (!out) {
    free(buf);
    return -1;
  }

  size_t w = 0;
  for (size_t r = 0; r < line_len; ++r) {
    if (buf[r] == '\r')
      continue;
    out[w++] = buf[r];
  }
  out[w] = '\0';

  free(buf);
  *out_line = out;
  return 0;
}

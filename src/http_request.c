#include "http_request.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
  const char *first_space = strchr(line, ' ');
  if (!first_space) {
    errno = EINVAL;
    return -1;
  }
  const char *second_space = strchr(first_space + 1, ' ');
  if (!second_space) {
    errno = EINVAL;
    return -1;
  }

  // method
  size_t method_len = (size_t)(first_space - line);
  if (method_len == 0 || method_len >= METHOD_LEN) {
    errno = ERANGE;
    return -1;
  }
  memcpy(out->method, line, method_len);
  out->method[method_len] = '\0';

  // target
  const char *target = first_space + 1;
  size_t target_len = (size_t)(second_space - target);
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
  const char *version = second_space + 1;
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

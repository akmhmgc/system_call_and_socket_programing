#include <errno.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string.h>

#include "http_calc.h"
#include "http_request.h"

static void expect_ok_str(const char *req, const char *expected) {
  RequestLine request_line = {0};
  errno = 0;
  ASSERT_EQ(parse_request_line_dup(req, &request_line), 0) << strerror(errno);

  char *out = nullptr;
  errno = 0;
  ASSERT_EQ(calc_eval_request_line_dup(&request_line, &out), 0)
      << strerror(errno);
  ASSERT_NE(out, nullptr);
  EXPECT_STREQ(out, expected);
  free(out);
  request_line_cleanup(&request_line);
}

static void expect_err(const char *req, int expect_errno) {
  RequestLine request_line = {0};
  errno = 0;
  if (parse_request_line_dup(req, &request_line) == -1) {
    EXPECT_EQ(errno, expect_errno);
    return;
  }

  char *out = nullptr;
  errno = 0;
  EXPECT_EQ(calc_eval_request_line_dup(&request_line, &out), -1);
  EXPECT_EQ(errno, expect_errno);
  EXPECT_EQ(out, nullptr);
  request_line_cleanup(&request_line);
}

TEST(Calc, HappyPath) {
  expect_ok_str("GET /calc?query=2+10 HTTP/1.1", "12");
  expect_ok_str("GET /calc?query=-1+2 HTTP/1.1", "1");
  expect_ok_str("GET /calc?query=0+0 HTTP/1.1", "0");
}

TEST(Calc, BadFormat) {
  expect_err("", EINVAL);
  expect_err("GET /bad?query=2+3 HTTP/1.1", EINVAL);
  expect_err("GET /calc?foo=2+3 HTTP/1.1", EINVAL);
  expect_err("GET /calc?query=2-3 HTTP/1.1", EINVAL);
  expect_err("GET /calc?query=+3 HTTP/1.1", EINVAL);
  expect_err("GET /calc?query=2+ HTTP/1.1", EINVAL);
}

TEST(Calc, Overflow) {
  expect_err("GET /calc?query=2147483647+1 HTTP/1.1", ERANGE);
  expect_err("GET /calc?query=-2147483648+-1 HTTP/1.1", ERANGE);
}

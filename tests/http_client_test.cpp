#include "../src/http_client.h"

#include <gtest/gtest.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>

TEST(HttpClientTest, BuildRequestDupSimple) {
  char *buf = nullptr;
  size_t len = 0;

  int result = build_request_dup(&buf, &len, "GET /path HTTP/1.1");
  ASSERT_EQ(0, result);
  ASSERT_NE(nullptr, buf);
  EXPECT_GT(len, 0);

  std::string expected = "GET /path HTTP/1.1\r\n";
  EXPECT_STREQ(expected.c_str(), buf);
  EXPECT_EQ(expected.length(), len);

  free(buf);
}

TEST(HttpClientTest, BuildRequestDupWithQuery) {
  char *buf = nullptr;
  size_t len = 0;

  int result = build_request_dup(&buf, &len, "GET /calc?query=1+2 HTTP/1.1");
  ASSERT_EQ(0, result);
  ASSERT_NE(nullptr, buf);

  std::string expected = "GET /calc?query=1+2 HTTP/1.1\r\n";
  EXPECT_STREQ(expected.c_str(), buf);

  free(buf);
}

TEST(HttpClientTest, BuildRequestDupNullOut) {
  size_t len = 0;

  int result = build_request_dup(nullptr, &len, "GET / HTTP/1.1");
  ASSERT_EQ(-1, result);
  EXPECT_EQ(EINVAL, errno);
}

TEST(HttpClientTest, BuildRequestDupNullReqline) {
  char *buf = nullptr;
  size_t len = 0;

  int result = build_request_dup(&buf, &len, nullptr);
  ASSERT_EQ(-1, result);
  EXPECT_EQ(EINVAL, errno);
}

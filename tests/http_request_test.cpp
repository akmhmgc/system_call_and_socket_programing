#include <gtest/gtest.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "http_request.h"
#include "http_request_io.h"

TEST(HttpRequestValidate, Happy) {
    RequestLine request_line = {0};
    ASSERT_EQ(parse_request_line_dup("GET /calc?query=1+2 HTTP/1.1", &request_line), 0) << strerror(errno);
    errno = 0;
    EXPECT_EQ(validate_request_common(&request_line), 0) << strerror(errno);
    request_line_cleanup(&request_line);
}

TEST(HttpRequestValidate, RejectMethod) {
    RequestLine request_line = {0};
    ASSERT_EQ(parse_request_line_dup("POST /calc?query=1+2 HTTP/1.1", &request_line), 0) << strerror(errno);
    errno = 0;
    EXPECT_EQ(validate_request_common(&request_line), -1);
    EXPECT_EQ(errno, EINVAL);
    request_line_cleanup(&request_line);
}

TEST(HttpRequestValidate, RejectVersion) {
    RequestLine request_line = {0};
    ASSERT_EQ(parse_request_line_dup("GET /calc?query=1+2 HTTP/2", &request_line), 0) << strerror(errno);
    errno = 0;
    EXPECT_EQ(validate_request_common(&request_line), -1);
    EXPECT_EQ(errno, EINVAL);
    request_line_cleanup(&request_line);
}

static int make_socketpair(int fds[2]) {
    return socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
}

TEST(HttpRequestReadLine, ReadSingleChunk) {
    int fds[2];
    ASSERT_EQ(make_socketpair(fds), 0) << strerror(errno);

    const char *line = "GET /calc?query=1+2 HTTP/1.1\r\n";
    ssize_t wn = write(fds[0], line, strlen(line));
    ASSERT_EQ(wn, (ssize_t)strlen(line));

    char *out = nullptr;
    errno = 0;
    ASSERT_EQ(read_request_line(fds[1], &out), 0) << strerror(errno);
    ASSERT_NE(out, nullptr);
    EXPECT_STREQ(out, "GET /calc?query=1+2 HTTP/1.1");
    free(out);

    close(fds[0]);
    close(fds[1]);
}

TEST(HttpRequestReadLine, ReadInMultipleChunks) {
    int fds[2];
    ASSERT_EQ(make_socketpair(fds), 0) << strerror(errno);

    const char *part1 = "GET /calc?query=1";
    const char *part2 = "+2 HTTP/1.1\r\n";
    ASSERT_EQ(write(fds[0], part1, strlen(part1)), (ssize_t)strlen(part1));
    ASSERT_EQ(write(fds[0], part2, strlen(part2)), (ssize_t)strlen(part2));

    char *out = nullptr;
    errno = 0;
    ASSERT_EQ(read_request_line(fds[1], &out), 0) << strerror(errno);
    ASSERT_NE(out, nullptr);
    EXPECT_STREQ(out, "GET /calc?query=1+2 HTTP/1.1");
    free(out);

    close(fds[0]);
    close(fds[1]);
}

TEST(HttpRequestReadLine, PeerClosedBeforeCRLF) {
    int fds[2];
    ASSERT_EQ(make_socketpair(fds), 0) << strerror(errno);

    const char *partial = "GET /calc"; // CRLF なしで閉じる
    ASSERT_EQ(write(fds[0], partial, strlen(partial)), (ssize_t)strlen(partial));
    close(fds[0]);

    char *out = nullptr;
    errno = 0;
    EXPECT_EQ(read_request_line(fds[1], &out), -1);
    EXPECT_EQ(out, nullptr);

    close(fds[1]);
}



#include <gtest/gtest.h>
#include <string.h>
#include <stdlib.h>

#include "http_response.h"

TEST(HttpServer, BuildRespDupEmptyBody) {
    char* out = nullptr;

    EXPECT_EQ(build_resp_dup(&out, ""), 0);
    ASSERT_NE(out, nullptr);

    EXPECT_NE(strstr(out, "HTTP/1.1 200 OK"), nullptr);
    EXPECT_NE(strstr(out, "Content-Length: 0"), nullptr);
    EXPECT_NE(strstr(out, "\r\n\r\n"), nullptr);
    
    free(out);
}

TEST(HttpServer, BuildRespDupWithNumbers) {
    char* out = nullptr;

    EXPECT_EQ(build_resp_dup(&out, "123"), 0);
    ASSERT_NE(out, nullptr);

    EXPECT_NE(strstr(out, "HTTP/1.1 200 OK"), nullptr);
    EXPECT_NE(strstr(out, "Content-Length: 3"), nullptr);
    EXPECT_NE(strstr(out, "\r\n\r\n123"), nullptr);

    free(out);
}

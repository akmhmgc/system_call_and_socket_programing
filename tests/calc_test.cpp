#include "../src/calc.h"

#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>

TEST(CalcTest, EvalSumSimple) {
  int result = 0;
  ASSERT_EQ(0, eval_sum("1+2", &result));
  EXPECT_EQ(3, result);
}

TEST(CalcTest, EvalSumNegativeNumbers) {
  int result = 0;
  ASSERT_EQ(0, eval_sum("-5+10", &result));
  EXPECT_EQ(5, result);
}

TEST(CalcTest, EvalSumLargeNumbers) {
  int result = 0;
  ASSERT_EQ(0, eval_sum("1000000+2000000", &result));
  EXPECT_EQ(3000000, result);
}

TEST(CalcTest, EvalSumInvalidExpression) {
  int result = 0;
  ASSERT_EQ(-1, eval_sum("invalid", &result));
  EXPECT_EQ(EINVAL, errno);
}

TEST(CalcTest, EvalSumNullExpression) {
  int result = 0;
  ASSERT_EQ(-1, eval_sum(nullptr, &result));
  EXPECT_EQ(EINVAL, errno);
}

TEST(CalcTest, EvalSumNullOutput) {
  ASSERT_EQ(-1, eval_sum("1+2", nullptr));
  EXPECT_EQ(EINVAL, errno);
}

TEST(CalcTest, ExtractQueryValueSimple) {
  char *result = nullptr;
  ASSERT_EQ(0, extract_query_value_dup("query=1+2", &result));
  ASSERT_NE(nullptr, result);
  EXPECT_STREQ("1+2", result);
  free(result);
}

TEST(CalcTest, ExtractQueryValueComplex) {
  char *result = nullptr;
  ASSERT_EQ(0, extract_query_value_dup("query=100+200", &result));
  ASSERT_NE(nullptr, result);
  EXPECT_STREQ("100+200", result);
  free(result);
}

TEST(CalcTest, ExtractQueryValueInvalidPrefix) {
  char *result = nullptr;
  ASSERT_EQ(-1, extract_query_value_dup("invalid=1+2", &result));
  EXPECT_EQ(EINVAL, errno);
}

TEST(CalcTest, ExtractQueryValueNullInput) {
  char *result = nullptr;
  ASSERT_EQ(-1, extract_query_value_dup(nullptr, &result));
  EXPECT_EQ(EINVAL, errno);
}

TEST(CalcTest, ExtractQueryValueNullOutput) {
  ASSERT_EQ(-1, extract_query_value_dup("query=1+2", nullptr));
  EXPECT_EQ(EINVAL, errno);
}

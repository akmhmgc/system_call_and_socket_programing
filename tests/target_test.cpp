# include "gtest/gtest.h"
extern "C" {
# include "target.h"
}

class fixtureName : public ::testing::Test {
protected:
    virtual void SetUp(){
    }
    virtual void TearDown(){
    }
};

TEST_F(fixtureName, testOk)
{
    EXPECT_EQ(1, function(10));
    EXPECT_EQ(0, function(11));
}
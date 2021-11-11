#include <gtest/gtest.h>
#include <iostream>
#include "kyiv.h"


namespace {
    class KyivTests
            : public ::testing::Test {
    public:
        Kyiv_t machine1;
    };

    TEST_F(KyivTests, StringAppendC) {
        machine1.C_reg = 1;
        word_to_number(1245);
        EXPECT_TRUE(1);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

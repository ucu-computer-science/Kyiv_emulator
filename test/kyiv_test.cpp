#include <gtest/gtest.h>
#include <iostream>
#include "kyiv.h"


namespace {
    class KyivTests
            : public ::testing::Test {
    public:
        Kyiv_t machine1;
    };

    TEST_F(KyivTests, add) {
        word_t num1 = 5;
        word_t num2 = 10;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'01'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[5]);
        EXPECT_EQ(got, 15);
        EXPECT_EQ(machine1.C_reg, 2);
    }

    TEST_F(KyivTests, sub) {
        word_t num1 = 13;
        word_t num2 = 11;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'02'0003'0004'0005ULL;
        machine1.kmem[3] = to_negative(num1);
        machine1.kmem[4] = num2;
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[5]);
        EXPECT_EQ(got, -24);
        EXPECT_EQ(machine1.C_reg, 2);
    }

    TEST_F(KyivTests, addcmd) {
        word_t num1 = 15;
        word_t num2 = 13;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'03'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = to_negative(num2);
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[5]);
        EXPECT_EQ(got, -28);
        EXPECT_EQ(machine1.C_reg, 2);
    }

    TEST_F(KyivTests, subabs) {
        word_t num1 = 13;
        word_t num2 = 15;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'06'0003'0004'0005ULL;
        machine1.kmem[3] = to_negative(num1);
        machine1.kmem[4] = num2;
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[5]);
        EXPECT_EQ(got, -2);
        EXPECT_EQ(machine1.C_reg, 2);
    }

//    TEST_F(KyivTests, addcyc) {
//        word_t num1 = 13;
//        word_t num2 = 15;
//        machine1.C_reg = 1;
//        machine1.kmem[1] = 0'07'0003'0004'0005ULL;
//        machine1.kmem[3] = to_negative(num1);
//        machine1.kmem[4] = num2;
//        machine1.execute_opcode();
//        word_t got = word_to_number(machine1.kmem[5]);
//        EXPECT_EQ(got, -2);
//        EXPECT_EQ(machine1.C_reg, 2);
//    }

    TEST_F(KyivTests, read_perfo_data) {
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'20'0003'0004'0000ULL;
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[3]);
        EXPECT_EQ(got, 1);
        EXPECT_EQ(machine1.C_reg, 2);
    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

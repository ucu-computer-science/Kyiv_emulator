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

    TEST_F(KyivTests, jmp_less_eq) {
        word_t num1 = 13;
        word_t num2 = 15;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'04'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.kmem[5] = 0'04'0004'0006'0007ULL;
        machine1.kmem[6] = to_negative(num2);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 5);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 6);
    }

    TEST_F(KyivTests, jmp_abs_less_eq) {
        word_t num1 = 13;
        word_t num2 = 15;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'05'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.kmem[5] = 0'05'0004'0006'0007ULL;
        machine1.kmem[6] = to_negative(num2);
        machine1.kmem[7] = 0'05'0006'0003'0011ULL;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 5);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 7);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 8);
    }

    TEST_F(KyivTests, jmp_eq) {
        word_t num1 = 13;
        word_t num2 = 15;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'16'0003'0004'0005ULL;
        machine1.kmem[2] = 0'16'0003'0005'0006ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.kmem[5] = num1;
        machine1.kmem[6] = to_negative(num2);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 2);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 6);
    }

    TEST_F(KyivTests, fork_neg) {
        word_t num1 = 13;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'31'0003'0004'0005ULL;
        machine1.kmem[4] = 0'31'0002'0005'0006ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[2] = to_negative(num1);
        machine1.kmem[5] = num1;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 4);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 6);
    }

    TEST_F(KyivTests, call_neg) {
        word_t num1 = 13;
        word_t num2 = 22;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'32'0003'0004'0005ULL;
        machine1.kmem[2] = 0'32'0005'0006'0007ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[5] = to_negative(num1);
        machine1.kmem[6] = num1;
        machine1.kmem[7] = num2;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 2);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, 7);
        EXPECT_EQ(machine1.P_reg, 6);
    }

    TEST_F(KyivTests, ret) {
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'30'0003'0004'0005ULL;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.C_reg, machine1.P_reg);
    }

    TEST_F(KyivTests, group_op_begin) {
        word_t num1 = 10;
        word_t num2 = 9;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'26'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.kmem[5] = num1;
        machine1.kmem[2] = 0'26'0003'0005'0006ULL;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.Loop_reg, 10);
        EXPECT_EQ(machine1.A_reg, 9);
        EXPECT_EQ(machine1.C_reg, 2);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.Loop_reg, 10);
        EXPECT_EQ(machine1.A_reg, 10);
        EXPECT_EQ(machine1.C_reg, 6);
    }

    TEST_F(KyivTests, group_op_end) {
        word_t num1 = 10;
        word_t num2 = 9;
        machine1.C_reg = 1;
        machine1.A_reg = 1;
        machine1.Loop_reg = 11;
        machine1.kmem[1] = 0'27'0003'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[4] = num2;
        machine1.kmem[5] = 0'27'0003'0007'0006ULL;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.A_reg, 11);
        EXPECT_EQ(machine1.C_reg, 5);
        machine1.execute_opcode();
        EXPECT_EQ(machine1.A_reg, 21);
        EXPECT_EQ(machine1.C_reg, 7);
    }

    TEST_F(KyivTests, F) {
        word_t num1 = 7;
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'34'0002'0004'0005ULL;
        machine1.kmem[3] = num1;
        machine1.kmem[2] = 0'26'0001'0003'0006ULL;
        machine1.execute_opcode();
        EXPECT_EQ(machine1.A_reg, 3);
        word_t got = machine1.kmem[5];
        EXPECT_EQ(got, 7);
    }

    TEST_F(KyivTests, stop) {
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'33'0002'0004'0005ULL;
        bool res = machine1.execute_opcode();
        EXPECT_FALSE(res);
        EXPECT_TRUE(machine1.T_reg);
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

    TEST_F(KyivTests, addcyc) {
        machine1.C_reg = 1;
        machine1.kmem[1] = 0'07'0003'0004'0005ULL;
        machine1.kmem[3] = (1ULL << 39); // 0b1000...00 -- 39 zeros after the 1
        machine1.kmem[4] = (1ULL << 39); // the same
        machine1.execute_opcode();
        word_t got = word_to_number(machine1.kmem[5]);
        EXPECT_EQ(got, 1);
        EXPECT_EQ(machine1.C_reg, 2);
    }

//    TEST_F(KyivTests, read_perfo_data) {
//        machine1.C_reg = 1;
//        machine1.kmem[1] = 0'20'0003'0004'0000ULL;
//        machine1.execute_opcode();
//        word_t got = word_to_number(machine1.kmem[3]);
//        EXPECT_EQ(got, 1);
//        EXPECT_EQ(machine1.C_reg, 2);
//    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

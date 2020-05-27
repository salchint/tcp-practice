#include "errorReturn.h"
//#include "../inc/errorReturn.c"
#include "protocol.h"
#include "protocol.c"
#include <iostream>


// Fake implementation
//void error_return(FILE* destination, enum PlayerErrorCodes code) {
    //fprintf(destination, "%s\n", playerErrorTexts[code]);
    //exit(code);
//}

// Fake implementation
//void error_return_dealer(FILE* destination, enum DealerErrorCodes code,
    //int dealerContext) {
    //fprintf(destination, "%s\n", dealerErrorTexts[code]);
    //if (dealerContext) {
        //exit(code);
    //}
    //_exit(code);
//}


#include "gtest/gtest.h"
//#include "gmock/gmock.h"



// Prototyping

// Fixture
class A4Suite : public ::testing::Test {
public:

public:
    A4Suite() {
    }

public:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// Tests
TEST_F(A4Suite, test00) {
    EXPECT_EQ(1, 1);
}

TEST_F(A4Suite, test_arg_chars) {
    EXPECT_EQ(E_CONTROL_OK, control_check_chars("ABC"));
    EXPECT_EQ(E_CONTROL_OK, control_check_chars("ABC DEF"));
    EXPECT_EQ(E_CONTROL_OK, control_check_chars("some other info"));
    EXPECT_EQ(E_CONTROL_OK, control_check_chars("A1 condition 364"));
    EXPECT_EQ(E_CONTROL_INVALID_INFO, control_check_chars("a\n"));
    EXPECT_EQ(E_CONTROL_INVALID_INFO, control_check_chars("a\rb"));
    EXPECT_EQ(E_CONTROL_INVALID_INFO, control_check_chars("a:b"));
}

TEST_F(A4Suite, test_port) {
    EXPECT_EQ(E_CONTROL_OK, control_check_port("1"));
    EXPECT_EQ(E_CONTROL_OK, control_check_port("65534"));
    EXPECT_EQ(E_CONTROL_OK, control_check_port("+1"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT, control_check_port("65535"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT, control_check_port("0"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT, control_check_port("-1"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT2, control_check_port("abc"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT2, control_check_port("12a3"));
    EXPECT_EQ(E_CONTROL_INVALID_INFO, control_check_port("123:"));
}


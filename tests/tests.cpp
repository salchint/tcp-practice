#include "errorReturn.h"
#include "protocol.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

//#include "errorReturn.c"
#include "protocol.c"

// Fake implementations
void error_return_control(enum ControlErrorCodes code) {
    fprintf(stderr, "%s\n", controlErrorTexts[code]);
    return;
}

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
    EXPECT_EQ(E_CONTROL_INVALID_PORT, control_check_port("abc"));
    EXPECT_EQ(E_CONTROL_INVALID_PORT, control_check_port("12a3"));
    EXPECT_EQ(E_CONTROL_INVALID_INFO, control_check_port("123:"));
}

TEST_F(A4Suite, test_open_connection) {
    struct sockaddr_in address;
    socklen_t length = sizeof(address);
    int port = 0;
    int socket = control_open_incoming_conn(&port);
    EXPECT_LT(0, socket);
    memset(&address, 0, length);
    EXPECT_EQ(0, getsockname(socket, (sockaddr*)(&address), &length));
    EXPECT_EQ(port, ntohs(address.sin_port));
    control_close_conn(socket);
}


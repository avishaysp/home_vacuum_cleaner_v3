// test_main.cpp (Test Suite Main)
#include <gtest/gtest.h>
#include "../src/common/logger.h"

int main(int argc, char **argv) {
    logger.setLogFileName("test_main_thread.log");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
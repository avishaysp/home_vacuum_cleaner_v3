#include "test_simulator.h"

TEST_F(SimulatorTest, TestSetProperties) {
    // This test uses public methods to check properties, without needing private access
    EXPECT_EQ(simulator.getHistoryLength(), static_cast<unsigned long>(0));
}

TEST_F(SimulatorTest, TestCalcScore) {
    // Indirectly using private member states initialized in SetUp
    EXPECT_EQ(simulator.calcScore(), static_cast<unsigned long>(5910));
}

TEST_F(SimulatorTest, TestMoveRightMove) {
    Location next_location = moveSimulator(Step::South);
    EXPECT_EQ(next_location, Location(1, 0));
}

TEST_F(SimulatorTest, TestUpdateDirtLevel) {
    size_t dirt_level = updateDirt();
    EXPECT_EQ(dirt_level, static_cast<unsigned long>(0));
}
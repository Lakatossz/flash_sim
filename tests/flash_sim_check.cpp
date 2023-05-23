#include <gtest/gtest.h>
#include <../flash_sim/include/flash/flash.h>

int add(int a, int b)
{
    return a + b;
}

auto flashMemory = new Flash_Memory();

TEST(initialization, positive)
{
ASSERT_EQ(EXIT_SUCCESS, flashMemory->Flash_Init());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return EXIT_SUCCESS;
}

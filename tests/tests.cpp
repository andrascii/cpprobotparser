#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    const int result = RUN_ALL_TESTS();

    std::cin.get();

    return result;
}
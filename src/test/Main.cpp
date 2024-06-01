#include <gtest/gtest.h>
#include <sese/Init.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::initCore(argc, argv);
    return RUN_ALL_TESTS();
}
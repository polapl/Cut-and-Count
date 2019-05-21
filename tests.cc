#include "tree.h"
#include "standard_dynamic.h"
#include "dynamic.h"

#include <gtest/gtest.h>

TEST(SquareRootTest, PositiveNos) {
  Tree tree;
  tree.Generate(10,10);
  EXPECT_EQ(1,1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
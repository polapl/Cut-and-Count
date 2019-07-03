#include "tree.h"
#include "standard_dynamic.h"
#include "dynamic.h"
#include "subset_view.h"
#include "disjoint_set.h"

#include <gtest/gtest.h>
using namespace std;

/*
TEST(SquareRootTest, PositiveNos) {
  Tree tree;
  tree.Generate(10,10);
  EXPECT_EQ(1,1);
}
*/

TEST(StandardDynamic_SimpleTriangle, StandardDynamic_SimpleTriangle) {
  Node c(0, true);
  Node b(1, false);
  Node a(2, true);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, b), 1),
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, c), 1),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  tree.AddNodeToAllBags(tree.root, a);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,1);
  
}

TEST(StandardDynamic_SimplePath, StandardDynamic_SimplePath) {
  Node a(0, true);
  Node b(1, false);
  Node c(2, true);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::FORGET_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, b), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::LEAF)
  };

  Tree tree(bags);
  tree.AddNodeToAllBags(tree.root, a);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

TEST(StandardDynamic_SimpleSquare, StandardDynamic_SimpleSquare) {
  Node a(3, true);
  Node b(0, true);
  Node c(2, false);
  Node d(1, false);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::MERGE),
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, c), 1),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF),
    new Bag(Bag::BagType::FORGET_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, d), 2),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, d), 2),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  tree.AddNodeToAllBags(tree.root, a);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

TEST(SimpleTriangle, SimpleTriangle) {
  Node a(0, true);
  Node b(1, false);
  Node c(2, true);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, a),
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, b), 1),
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, c), 1),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,1);
}



TEST(SimplePath, SimplePath) {
  Node a(0, true);
  Node b(1, false);
  Node c(2, true);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::FORGET_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, b), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  tree.DotTransitionGraph("example.dot");
  printf("TU TERAZ\n");
  tree.tree_width = 3;
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

TEST(SimpleSquare, SimpleSquare) {
  Node a(0, true);
  Node b(1, true);
  Node c(2, false);
  Node d(3, false);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, a),
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::MERGE),
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, c), 1),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF),
    new Bag(Bag::BagType::FORGET_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(a, d), 2),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, d), 2),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  tree.DotTransitionGraph("example.dot");
  printf("TU TERAZ\n");
  tree.tree_width = 3;
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
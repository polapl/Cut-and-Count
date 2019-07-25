#include "tree.h"
#include "standard_dynamic.h"
#include "standard_hamiltonian.h"
#include "dynamic.h"
#include "subset_view.h"
#include "disjoint_set.h"

#include <gtest/gtest.h>
using namespace std;

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
  tree.AddNodeToAllBags(tree.root, a, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree);
  int res = dyn->Compute();
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
  tree.AddNodeToAllBags(tree.root, a, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree);
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
  tree.AddNodeToAllBags(tree.root, a, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardDynamic* dyn= new StandardDynamic(&tree);
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
  tree.tree_width = 3;
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
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
  tree.tree_width = 3;
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
  EXPECT_EQ(res,2);
}
/*
TEST(SmallTests_HighProbability, SmallTests_HighProbability) {
  int number_of_tests = 10;
  while(number_of_tests--) {
    Tree tree(3, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 30);
    tree.IntroduceEdges(50);
    tree.DotTransitionGraph("example_dyn.dot");
    
    Dynamic* dyn= new Dynamic(&tree, 50);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardDynamic* standard_dyn= new StandardDynamic(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(SmallTests_LowProbability, SmallTests_LowProbability) {
  int number_of_tests = 10;
  while(number_of_tests--) {
    Tree tree(3, 100);
    tree.Generate(5, 20);
    tree.IntroduceEdges(50);
    tree.DotTransitionGraph("example_dyn.dot");
    
    Dynamic* dyn= new Dynamic(&tree, 3);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardDynamic* standard_dyn= new StandardDynamic(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(BigTests_HighProbability, BigTests_HighProbability) {
  int number_of_tests = 1;
  while(number_of_tests--) {
    Tree tree(4, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 50);
    tree.IntroduceEdges(80);
    tree.DotTransitionGraph("example_dyn.dot");
    
    Dynamic* dyn= new Dynamic(&tree, 50);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardDynamic* standard_dyn= new StandardDynamic(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(SmallTests_HighProbability, SmallTests_HighProbability_OnlyStandard) {
  int number_of_tests = 1;
  while(number_of_tests--) {
    Tree tree(3, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 50);
    tree.IntroduceEdges(50);
    tree.DotTransitionGraph("example_dyn.dot");
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardDynamic* standard_dyn= new StandardDynamic(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    // printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_standard, res_standard);
  }
}
*/
/*
TEST(StandardHamiltonian_SimpleTriangle, StandardHamiltonian_SimpleTriangle) {
  Node a(0, false);
  Node b(1, false);
  Node c(2, false);
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
  Node d(3, false);
  tree.PrepareBeforeStandardHamiltonian(d, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardHamiltonian* dyn = new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, true);
}

TEST(StandardHamiltonian_SimplePath, StandardHamiltonian_SimplePath) {
  Node a(0, false);
  Node b(1, false);
  Node c(2, false);
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
  Node d(3, false);
  tree.PrepareBeforeStandardHamiltonian(d, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardHamiltonian* dyn= new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, false);
}
*/
TEST(StandardHamiltonian_SimpleSquare, StandardHamiltonian_SimpleSquare) {
  Node a(3, true);
  Node b(0, true);
  Node c(2, false);
  Node d(1, false);
  std::vector<Bag*> bags {
    new Bag(Bag::BagType::FORGET_NODE, a),
    new Bag(Bag::BagType::FORGET_NODE, b),
    new Bag(Bag::BagType::MERGE),
    new Bag(Bag::BagType::FORGET_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(c, a), 1),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, c), 1),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, c),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF),
    new Bag(Bag::BagType::FORGET_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(d, a), 2),
    new Bag(Bag::BagType::INTRODUCE_EDGE, std::make_pair(b, d), 2),
    new Bag(Bag::BagType::INTRODUCE_NODE, a),
    new Bag(Bag::BagType::INTRODUCE_NODE, d),
    new Bag(Bag::BagType::INTRODUCE_NODE, b),
    new Bag(Bag::BagType::LEAF)
  };
  Tree tree(bags);
  Node e(4, false);
  tree.PrepareBeforeStandardHamiltonian(e, false);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3; // 4 ?
  StandardHamiltonian* dyn= new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

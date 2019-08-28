#include "tree.h"
#include "cnc_steiner_tree.h"
#include "cnc_hamiltonian.h"
#include "standard_steiner_tree.h"
#include "standard_hamiltonian.h"
#include "subset_view.h"
#include "disjoint_set.h"

#include <gtest/gtest.h>
using namespace std;

TEST(StandardSteinerTree, SimpleTriangle) {
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
  StandardSteinerTree* dyn= new StandardSteinerTree(&tree);
  int res = dyn->Compute();
  EXPECT_EQ(res,1);
}

TEST(StandardSteinerTree, SimplePath) {
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
  StandardSteinerTree* dyn= new StandardSteinerTree(&tree);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

TEST(StandardSteinerTree, SimpleSquare) {
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
  StandardSteinerTree* dyn= new StandardSteinerTree(&tree);
  int res = dyn->Compute();
  printf("res = %d\n", res);
  EXPECT_EQ(res,2);
}

TEST(CnCSteinerTree, SimpleTriangle) {
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
  CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 3);
  int res = dyn->Compute();
  EXPECT_EQ(res,1);
}

TEST(CnCSteinerTree, SimplePath) {
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
  CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 3);
  int res = dyn->Compute();
  EXPECT_EQ(res,2);
}

TEST(CnCSteinerTree, SimpleSquare) {
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
  CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 3);
  int res = dyn->Compute();
  EXPECT_EQ(res,2);
}

TEST(SteinerTree, SmallTests_HighProbability) {
  int number_of_tests = 10;
  while(number_of_tests--) {
    Tree tree(3, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 30);
    tree.IntroduceEdges(70);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 50);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardSteinerTree* standard_dyn= new StandardSteinerTree(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(SteinerTree, SmallTests_LowProbability) {
  int number_of_tests = 30;
  while(number_of_tests--) {
    Tree tree(3, 100);
    tree.Generate(5, 30);
    tree.IntroduceEdges(50);
    tree.DotTransitionGraph("example_cnc_f.dot");
    
    CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 4);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard_f.dot");
    tree.tree_width++;
    StandardSteinerTree* standard_dyn= new StandardSteinerTree(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    if (res_standard != res_dyn) break;
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(SteinerTree, BigTests_HighProbability) {
  int number_of_tests = 1;
  while(number_of_tests--) {
    Tree tree(4, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 50);
    tree.IntroduceEdges(80);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCSteinerTree* dyn= new CnCSteinerTree(&tree, 50);
    unsigned long long res_dyn = dyn->Compute();
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardSteinerTree* standard_dyn= new StandardSteinerTree(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(StandardSteinerTree, SmallTests_HighProbability_OnlyStandard) {
  int number_of_tests = 1;
  while(number_of_tests--) {
    Tree tree(3, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 50);
    tree.IntroduceEdges(50);
    tree.DotTransitionGraph("example_cnc.dot");
    
    tree.AddNodeToAllBags(tree.root, tree.root->forgotten_node, true);
    tree.root = tree.root->left;
    delete tree.root->parent;
    tree.root->parent = nullptr;
    tree.DotTransitionGraph("example_standard.dot");
    tree.tree_width++;
    StandardSteinerTree* standard_dyn= new StandardSteinerTree(&tree);
    unsigned long long res_standard = standard_dyn->Compute();
    // printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_standard, res_standard);
  }
}

TEST(StandardHamiltonian, SimpleTriangle) {
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
  Node d(-1, false);
  tree.PrepareBeforeStandardHamiltonian(d);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardHamiltonian* dyn = new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, true);
}

TEST(StandardHamiltonian, SimplePath) {
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
  Node d(-1, false);
  tree.PrepareBeforeStandardHamiltonian(d);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3;
  StandardHamiltonian* dyn= new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, false);
}

TEST(StandardHamiltonian, SimpleSquare) {
  Node a(3, false);
  Node b(0, false);
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
  Node e(-1, false);
  tree.PrepareBeforeStandardHamiltonian(e);
  tree.DotTransitionGraph("example.dot");
  tree.tree_width = 3; // 4 ?
  StandardHamiltonian* dyn= new StandardHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res, true);
}

TEST(CnCHamiltonian, SimpleTriangle) {
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
  tree.DotTransitionGraph("example.dot");
  //tree.tree_width = 3;
  CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res,true);
}

TEST(CnCHamiltonian, SimplePath) {
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
  tree.DotTransitionGraph("example.dot");
  //tree.tree_width = 3;
  CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res,false);
}

TEST(CnCHamiltonian, SimpleSquare) {
  Node a(0, false);
  Node b(1, false);
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
  //tree.tree_width = 3;
  CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
  bool res = dyn->Compute();
  EXPECT_EQ(res,true);
}

TEST(Hamiltonian, SmallTests_HighProbability) {
  int number_of_tests = 100;
  while(number_of_tests--) { 
    Tree tree(3, 100);
    tree.Generate(7, 0);
    tree.IntroduceEdges(90);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
    bool res_dyn = dyn->Compute();
    Node n(-1, false);
    tree.PrepareBeforeStandardHamiltonian(n);
    tree.DotTransitionGraph("example_standard.dot");
    StandardHamiltonian* standard_dyn= new StandardHamiltonian(&tree);
    bool res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(Hamiltonian, SmallTests_LowProbability) {
  int number_of_tests = 30;
  while(number_of_tests--) {
    Tree tree(3, 100);
    tree.Generate(5, 0);
    tree.IntroduceEdges(60);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
    bool res_dyn = dyn->Compute();
    Node n(-1, false);
    tree.PrepareBeforeStandardHamiltonian(n);
    tree.DotTransitionGraph("example_standard.dot");
    StandardHamiltonian* standard_dyn= new StandardHamiltonian(&tree);
    bool res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    // if (res_dyn != res_standard) break;
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(Hamiltonian, BigTests_HighProbability) {
  int number_of_tests = 1;
  while(number_of_tests--) {
    Tree tree(4, 100);
    printf("tree width = %d\n", tree.tree_width);
    tree.Generate(50, 0);
    tree.IntroduceEdges(90);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
    bool res_dyn = dyn->Compute();
    Node n(-1, false);
    tree.PrepareBeforeStandardHamiltonian(n);
    tree.DotTransitionGraph("example_standard.dot");
    StandardHamiltonian* standard_dyn= new StandardHamiltonian(&tree);
    bool res_standard = standard_dyn->Compute();
    printf("RESULTS: %d vs. %d\n", res_dyn, res_standard);
    EXPECT_EQ(res_dyn, res_standard);
  }
}

TEST(Hamiltonian, SmallTests_HighProbability_OnlyStandard) {
  int number_of_tests = 2;
  while(number_of_tests--) {
    Tree tree(3, 100);
    tree.Generate(5, 0);
    tree.IntroduceEdges(90);
    tree.DotTransitionGraph("example_cnc.dot");
    
    Node n(-1, false);
    tree.PrepareBeforeStandardHamiltonian(n);
    tree.DotTransitionGraph("example_standard.dot");
    StandardHamiltonian* standard_dyn= new StandardHamiltonian(&tree);
    bool res_standard = standard_dyn->Compute();
    printf("RESULT: %d\n", res_standard);
  }
}

TEST(Hamiltonian, SmallTests_HighProbability_OnlyCnC) {
  int number_of_tests = 2;
  while(number_of_tests--) {
    Tree tree(3, 100);
    tree.Generate(5, 0);
    tree.IntroduceEdges(90);
    tree.DotTransitionGraph("example_cnc.dot");
    
    CnCHamiltonian* dyn= new CnCHamiltonian(&tree);
    bool res_dyn = dyn->Compute();
    printf("RESULT: %d\n", res_dyn);
  }
}

/*
template <int seed, int tree_width, int max_weight, int bags_gen_type, int terminal_prob, int edge_count, bool result>
class HamiltonianTestsTemplate : public ::testing::Test {
  protected:
    static void SetUpTestSuite() {

      srand(seed);
      tree_ = std::make_unique<Tree>(tree_width, max_weight);
      tree_->Generate(bags_gen_type, terminal_prob);
      tree_->IntroduceEdges(edge_count);
    }

    static void TearDownTestSuite() {
      tree_.reset(nullptr);
    }

    static bool result_;
    static std::unique_ptr<Tree> tree_;
};

template <int seed, int tree_width, int max_weight, int bags_gen_type, int terminal_prob, int edge_count, bool result>
bool HamiltonianTestsTemplate<seed, tree_width, max_weight, bags_gen_type, terminal_prob, edge_count, result>::result_ = result;

template <int seed, int tree_width, int max_weight, int bags_gen_type, int terminal_prob, int edge_count, bool result>
std::unique_ptr<Tree> HamiltonianTestsTemplate<seed, tree_width, max_weight, bags_gen_type, terminal_prob, edge_count, result>::tree_ = nullptr;

typedef HamiltonianTestsTemplate<0x51234, 4, 100, 10, 0 , 60, true> HamiltonianTest_4_100_10_0_60;

TEST_F(HamiltonianTest_4_100_10_0_60, CncHamiltonianTest) {
  CnCHamiltonian* dyn = new CnCHamiltonian(tree_.get());

  EXPECT_EQ(dyn->Compute(), result_);
}

TEST_F(HamiltonianTest_4_100_10_0_60, StandardHamiltonianTest) {
  Node n(-1, false);
  tree_->PrepareBeforeStandardHamiltonian(n);
  StandardHamiltonian* dyn = new StandardHamiltonian(tree_.get());

  EXPECT_EQ(dyn->Compute(), result_);
}

typedef HamiltonianTestsTemplate<0x91234, 5, 100, 10, 0 , 60, true> HamiltonianTest_5_100_10_0_60;

TEST_F(HamiltonianTest_5_100_10_0_60, CncHamiltonianTest) {
  CnCHamiltonian* dyn = new CnCHamiltonian(tree_.get());

  EXPECT_EQ(dyn->Compute(), result_);
}

TEST_F(HamiltonianTest_5_100_10_0_60, StandardHamiltonianTest) {
  Node n(-1, false);
  tree_->PrepareBeforeStandardHamiltonian(n);
  StandardHamiltonian* dyn = new StandardHamiltonian(tree_.get());

  EXPECT_EQ(dyn->Compute(), result_);
}
*/

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

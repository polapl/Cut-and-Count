#pragma once

#include "tree.h"

class StandardSteinerTree {
 public:
  StandardSteinerTree(Tree* tree) : tree(tree) {}
  unsigned long long Compute();
  Tree* tree;
};

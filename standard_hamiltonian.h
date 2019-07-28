#pragma once

#include "tree.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>

using namespace std;

class StandardHamiltonian {
 public:
  StandardHamiltonian(Tree* tree) : tree(tree) {}
  bool Compute();
  Tree* tree;
};

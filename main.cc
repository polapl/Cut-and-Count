#include <iostream>
#include <cstdlib>
#include <ctime>

#include "tree.h"
#include "dynamic.h"

using namespace std;

void init(int seed) {
  cout << "Initializing seed: " << seed << endl;
  srand(seed);
}

int main(int argc, char** argv) {
  Tree tree(5, 100);
  tree.Generate(5, 50);
  tree.IntroduceEdges(50);
  tree.DotTransitionGraph("example.dot");
  Dynamic* dyn= new Dynamic(&tree, 3);
  int res = dyn->Compute();
  cout << res << endl;
  return 0;
}
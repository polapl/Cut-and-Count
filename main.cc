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
  // init(atoi(argv[1]));
  Tree tree;
  // size, probabilty
  tree.Generate(1000, 10);
  tree.IntroduceEdges(10);
  tree.DotTransitionGraph("example.dot");
  return 0;
}
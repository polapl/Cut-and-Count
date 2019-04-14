#include "dynamic.h"

#define dynamic_results vector<vector<vector<int>>>

void Recursive()

int Dynamic::Compute() {
  dynamic_results vec = dynamic_results(this->tree_size);
  for(int i=0; i<tree_size; i++) {
    vec[i] = vector<vector<int>>();
    for(int j=0; i<this->l; j++){
      vec[i][j] = vector<int>();
      for(int f; f<3^(k-1); f++) {
        vec[i][j][f] = 0;
      }
    }
  }
  Recursive(vec);
}
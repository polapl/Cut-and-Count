#include "standard_dynamic.h"

#include "tree.h"
#include "subset_view.h"
#include "disjoint_set.h"

using namespace std;

typedef std::vector<std::vector<std::vector<unsigned long long>>> dynamic_results ;

typedef long long ll_t;
ll_t pow(const ll_t x, const ll_t y) {
  return y ? (y & 1 ? x : 1) * pow(x * x, y >> 1) : 1;
}

//k = max width, l = max edges in Steiner tree
void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);

  SubsetView<Node*> sset(bag->nodes);
  for(; sset; ++sset) {
    vector<Node*> sset_materialize = sset.materialize();

    printf("\nsset_materialize:");
    for(auto& it: sset_materialize) {
      printf("  %d ", it->value);
    }
    printf("\n");

    PartitionView<Node*> pset(sset_materialize);
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      printf("vec[%d][%lld][%lld]\n", bag->id, sset.hash(), it.hash());
      it.print();
      switch(bag->type) {
        case Bag::LEAF:
        {
          vec[bag->id][0][0] = 1000000;
          vec[bag->id][sset.hash()][it.hash()] = 0;
          //printf("LEAF[%d][%d][%d]\n", bag->id, x, p); 
          break;
        }
        case Bag::INTRODUCE_NODE:
        {
          if (bag->introduced_node->terminal && !sset.is_present(bag->introduced_node)) {
            vec[bag->id][sset.hash()][it.hash()] = 1000000;
          } else if (!it.singleton(bag->introduced_node)) {
            vec[bag->id][sset.hash()][it.hash()] = 1000000;
          } else if (sset.is_present(bag->introduced_node)) {
            vector<Node*> sset_materialize_copy = sset_materialize;
            for (auto el_it = sset_materialize_copy.begin(); el_it != sset_materialize_copy.end(); ++el_it) {
              if (*el_it == bag->introduced_node) {
                sset_materialize_copy.erase(el_it);
                break;
              }
            }
            SubsetView<Node*> subset_without_node(sset_materialize_copy);
            auto partition_without_node = it;
            partition_without_node.remove_singleton(bag->introduced_node);
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][subset_without_node.hash()][partition_without_node.hash()];
          }
          //printf("INTRODUCE_NODE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::FORGET_NODE:
        {
          //printf("FORGET_NODE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::MERGE:
        {
          //printf("MERGE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          //printf("INTRODUCE_EDGE[%d][%d][%d]\n", bag->id, x, p);
          break;
        }
        default:
          printf("DEFAULT\n");
      } 
    }
  }

}

int StandardDynamic::Compute() {
  dynamic_results vec = std::vector<std::vector<std::vector<unsigned long long>>>(this->tree->GetTreeSize() + 2);
  int k = this->tree->GetTreeWidth();
  for(int i=0; i <= this->tree->GetTreeSize(); i++) {
    vec[i] = std::vector<std::vector<unsigned long long int>>(pow(2, k) + 1);
    for(int j=0; j <= pow(2, k); j++){
      vec[i][j] = std::vector<unsigned long long int>(pow(k, k) + 1);
      for(int f=0; f <= pow(k, k); f++) {
        vec[i][j][f] = 0;
      }
    }
  }
  recursive(vec, this->tree->GetTreeWidth(), this->l, this->tree->root);
  return 0;
}


/*
sset_materialize:  2   1 
vec[4][3][2]
part: 0
2 1 
vec[4][3][2] // to sie powtarza - czemu?
part: 0
2 1 
vec[4][3][3]
part: 0
1 
part: 1
2 
vec[4][3][4]
part: 0 // to powinno byc usuniete

part: 1
2 1 
*/
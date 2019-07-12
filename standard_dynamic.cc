#include "standard_dynamic.h"

#include <cmath>

#include <array>
#include <unordered_map>
#include "tree.h"
#include "disjoint_set.h"
#include "subset_view.h"

using namespace std;

const unsigned long long int INF = 1000000;

typedef unordered_map<size_t, unordered_map<size_t, unordered_map<size_t, unsigned long long>>> dynamic_results;

// Used for Bag of INTRODUCE_EDGE type.
// If we introduce a new edge that connects two different partition subsets,
// we have to merge them.
void merge_child_partitions(const Node& first, const Node& second, dynamic_results& vec, Bag* t) {
  SubsetView<Node> sset(t->nodes);
  for(; sset; ++sset) {
    if (!sset.is_present(first) || !sset.is_present(second))
      continue;
    vector<Node> sset_materialize = sset.materialize();
    PartitionView<Node> pset(sset_materialize);
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      int p1 = it.partition(first);
      int p2 = it.partition(second);
      if (p1 == p2) continue;
      PartitionView<Node>::iterator copy = it;
      copy.merge(p1, p2);
      vec[t->parent->id][sset.hash()][copy.hash()] = min(
        vec[t->parent->id][sset.hash()][copy.hash()],
        vec[t->id][sset.hash()][it.hash()] + 1
      );
    }
  }
}

//k = max width, l = max edges in Steiner tree
void recursive(dynamic_results &vec, int k, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, bag->left);
  recursive(vec, k, bag->right);

  bool one_edge_introduced = false;

  SubsetView<Node> sset(bag->nodes);
  // Iterate through all subsets of bag->nodes
  for(; sset; ++sset) {
    vector<Node> sset_materialize = sset.materialize();

    PartitionView<Node> pset(sset_materialize);
    // Iterate through all possible partitions
    for (auto it = pset.begin(); it != pset.end(); ++it) {

      switch(bag->type) {
        case Bag::LEAF:
        {
          vec[bag->id][sset.hash()][it.hash()] = (unsigned long long) 0;
          //printf("LEAF[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(), vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::INTRODUCE_NODE:
        {
          if (bag->introduced_node.terminal && !sset.is_present(bag->introduced_node)) {
            vec[bag->id][sset.hash()][it.hash()] = INF;
          } else if (sset.is_present(bag->introduced_node) && !it.singleton(bag->introduced_node)) {
            vec[bag->id][sset.hash()][it.hash()] = INF;
          } else if (sset.is_present(bag->introduced_node)) {
            auto partition_without_node = it;
            partition_without_node.remove_singleton(bag->introduced_node);
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_without_el(bag->introduced_node)][partition_without_node.hash()];
          } else {
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_without_el(bag->introduced_node)][it.hash()];
          }
          //printf("INTRODUCE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(), vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::FORGET_NODE:
        {
          vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_with_el(bag->forgotten_node, false)][it.hash()];
          int max_part = it.max_partition();
          for(int i=1; i<=max_part; i++) {
            PartitionView<Node>::iterator partition_copy = it;
            partition_copy.add_to_partition(bag->forgotten_node, i);
            unsigned long long tymczasowy = sset.hash_with_el(bag->forgotten_node, true);
            vec[bag->id][sset.hash()][it.hash()] = 
              min(vec[bag->id][sset.hash()][it.hash()],
                  vec[bag->left->id][tymczasowy][partition_copy.hash()]);
          }
          //printf("FORGET_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
          //       vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::MERGE:
        {
          // part1 and part2 are used to iterate through all possible pairs that:
          // - do not create an cycle
          // - sum up to pset
          PartitionView<Node> part1 = PartitionView<Node>(sset_materialize);
          PartitionView<Node> part2 = PartitionView<Node>(sset_materialize);
          // Check whether after merging part1 and part2 we will end up with cycle
          for (auto part_it1 = part1.begin(); part_it1 != part1.end(); ++part_it1) {
            DisjointSet<int> disjoint_set;
            for (const auto& s_m_it : sset_materialize) {
              disjoint_set.add(s_m_it.value);
            }
            const auto& distribution1 = part_it1.distribution();
            for (auto distr_it: distribution1) {
              for (int s = 1; s < distr_it.second.size(); s++) {
                disjoint_set.join(distr_it.second[0].value, distr_it.second[s].value);
              }
            }
            for (auto part_it2 = part2.begin(); part_it2 != part2.end(); ++part_it2) {
              bool cycle = false;
              DisjointSet<int> disjoint_set_copy = disjoint_set;
              const auto& distribution2 = part_it2.distribution();
              for (auto distr_it: distribution2) {
                for (int s=1; s<distr_it.second.size(); s++) {
                  if (disjoint_set_copy.find(distr_it.second[0].value) ==
                      disjoint_set_copy.find(distr_it.second[s].value)) {
                        cycle = true;
                        break;
                      }
                  disjoint_set_copy.join(distr_it.second[0].value, distr_it.second[s].value);
                }
              }
              if (!cycle) {
                bool equal = true;
                // Check whether x,y in the same part in pset => find(x) == find(y)
                auto pset_distribution = it.distribution();
                for (const auto& map_it : pset_distribution) {
                  for (int s = 1; s < map_it.second.size(); s++) {
                    if (disjoint_set_copy.find(map_it.second[0].value) ==
                        disjoint_set_copy.find(map_it.second[s].value)) continue;
                    equal = false;
                  }
                }
                if (!equal) continue;
                map<DisjointSet<int>::Representative, vector<Node>> merged_parts;
                for (const auto& s_m_it : sset_materialize) {
                  merged_parts[disjoint_set_copy.find(s_m_it.value)].push_back(s_m_it);
                }
                for (const auto& merged_parts_it: merged_parts) {
                  for (int s = 1; s < merged_parts_it.second.size(); s++) {
                    Node el1 = merged_parts_it.second[0];
                    Node el2 = merged_parts_it.second[s];
                    if (it.partition(el1) != it.partition(el2)) equal = false;
                  }
                }
                if (!equal) continue;
                vec[bag->id][sset.hash()][it.hash()] = min(vec[bag->id][sset.hash()][it.hash()],
                  vec[bag->left->id][sset.hash()][part_it1.hash()] +
                  vec[bag->right->id][sset.hash()][part_it2.hash()]);
              }
            }
          }
          //printf("MERGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
          //       vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          // If we cannot take this edge to solution because
          // one of its endpoints is not in the current subset.
          if (!sset.is_present(bag->introduced_edge.first) || !sset.is_present(bag->introduced_edge.second)) {
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash()][it.hash()];
            break;
          }
          int part_a = it.partition(bag->introduced_edge.first);
          int part_b = it.partition(bag->introduced_edge.second);
          // If we cannot take this edge to solution because
          // its endpoints are in different partitions.
          if (part_a != part_b) {
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash()][it.hash()];
            break;
          }
          // When we may take the edge to the solution.
          if (!one_edge_introduced) {
            //printf("current: %d\n", vec[bag->id][sset.hash()][it.hash()]);
            merge_child_partitions(bag->introduced_edge.first, bag->introduced_edge.second, vec, bag->left);
            one_edge_introduced = true;
          }
          //printf("INTRODUCE_EDGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
          //       vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
      } 
    }
  }
}
  

unsigned long long StandardDynamic::Compute() {
  int A = this->tree->GetTreeSize() + 1;
  int k = this->tree->GetTreeWidth();
  int B = pow(2, k + 2) + 1;
  int C = pow(k, k + 2) + 1;
  dynamic_results vec;

  for (int i=0; i<A; i++) {
    for(int j=0; j<B; j++) {
      for (int k=0; k<C; k++) {
        vec[i][j][k] = INF;
      }
    }
  }

  recursive(vec, this->tree->GetTreeWidth(), this->tree->root);
  unsigned long long res = vec[this->tree->root->id][1][1];
  //printf("MAX: %d %d %d\n", A, B, C);
  return res;
}

#include "standard_dynamic.h"

#include <cmath>

#include "tree.h"
#include "disjoint_set.h"
#include "subset_view.h"

using namespace std;

const unsigned long long int INF = 1000000;

typedef std::vector<std::vector<std::vector<unsigned long long>>> dynamic_results ;


void merge_child_partitions(Node* first, Node* second, dynamic_results& vec, Bag* t) {
  //printf("merge_child_partitions\n");
  SubsetView<Node*> sset(t->nodes);
  for(; sset; ++sset) {
    if (!sset.is_present(first) || !sset.is_present(second))
      continue;
    vector<Node*> sset_materialize = sset.materialize();
    printf("sset_materialize: ");
    for(const auto& it: sset_materialize) {
      printf("%d ", *it);
    }
    printf("\n");
    printf("FORGET NODE: %d\n", t->parent->forgotten_node);
    PartitionView<Node*> pset(sset_materialize);
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      int p1 = it.partition(first);
      int p2 = it.partition(second);
      if (p1 == p2) continue;
      PartitionView<Node*>::iterator copy = it;
      printf("BEFORE: ");
      copy.print();
      copy.merge(p1, p2);
      printf("AFTER: ");
      copy.print();
      vec[t->parent->id][sset.hash()][copy.hash()] = min(
        vec[t->parent->id][sset.hash()][copy.hash()],
        vec[t->id][sset.hash()][it.hash()] + 1
      );
    }
  }
}

//k = max width, l = max edges in Steiner tree
void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);

  SubsetView<Node*> sset(bag->nodes);
  for(; sset; ++sset) {
    bool one_edge_introduced = false;
    vector<Node*> sset_materialize = sset.materialize();

    printf("\nsset_materialize %d:", bag->id);
    for(auto& it: sset_materialize) {
      printf("  %d ", it->value);
    }
    printf("\n");

    PartitionView<Node*> pset(sset_materialize);
    for (auto it = pset.begin(); it != pset.end(); ++it) {
      //printf("vec[%d][%lld][%lld]\n", bag->id, sset.hash(), it.hash());
      it.print();
      switch(bag->type) {
        case Bag::LEAF:
        {
          //vec[bag->id][0][0] = INF;
          vec[bag->id][sset.hash()][it.hash()] = 0;
          printf("LEAF[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(), vec[bag->id][sset.hash()][it.hash()]); 
          break;
        }
        case Bag::INTRODUCE_NODE:
        {
          vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_without_el(bag->introduced_node)][it.hash()];
          if (bag->introduced_node->terminal && !sset.is_present(bag->introduced_node)) {
            //printf("NIE SPELNIA WARUNKOW\n");
            vec[bag->id][sset.hash()][it.hash()] = INF;
          } else if (sset.is_present(bag->introduced_node) && !it.singleton(bag->introduced_node)) {
            //printf("NIE SINGLETON\n");
            vec[bag->id][sset.hash()][it.hash()] = INF;
          } else if (sset.is_present(bag->introduced_node)) {
            auto partition_without_node = it;
            partition_without_node.remove_singleton(bag->introduced_node);
            //printf("czemu INF?\n");
            //printf("%d %lld %lld\n", bag->left->id, sset.hash_without_el(bag->introduced_node), partition_without_node.hash());
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_without_el(bag->introduced_node)][partition_without_node.hash()];
          } 
          printf("INTRODUCE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(), vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::FORGET_NODE:
        {
          vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash_with_el(bag->forgotten_node, false)][it.hash()];
          int max_part = it.max_partition();
          for(int i=1; i<=max_part; i++) {
            PartitionView<Node*>::iterator partition_copy = it;
            partition_copy.add_to_partition(bag->forgotten_node, i);
            unsigned long long tymczasowy = sset.hash_with_el(bag->forgotten_node, true);
            printf("partition el is added: %d\n", i);
            printf("new part: ");
            partition_copy.print();
            printf("full hash: %lld, part hash: %lld\n", tymczasowy, partition_copy.hash());
            printf("%d %lld %lld\n", bag->left->id, tymczasowy, partition_copy.hash());
            vec[bag->id][sset.hash()][it.hash()] = 
              min(vec[bag->id][sset.hash()][it.hash()],
                  vec[bag->left->id][tymczasowy][partition_copy.hash()]);
          }
          printf("FORGET_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
                 vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::MERGE:
        {
          // part1 and part2 are used to iterate through all possible pairs that:
          // - do not create an cycle
          // - sum up to pset
          PartitionView<Node*> part1 = PartitionView<Node*>(sset_materialize);
          PartitionView<Node*> part2 = PartitionView<Node*>(sset_materialize);
          for (auto part_it1 = part1.begin(); part_it1 != part1.end(); ++part_it1) {
            DisjointSet<int> disjoint_set;
            for (const auto& s_m_it : sset_materialize) {
              disjoint_set.add(s_m_it->value);
            }
            // disjoint_set.Create(sset_materialize.begin(), sset_materialize.end());
            const auto& distribution1 = part_it1.distribution();
            // Merge sets from part1.
            for (auto distr_it: distribution1) {
              for (int s = 1; s < distr_it.second.size(); s++) {
                disjoint_set.join(distr_it.second[0]->value, distr_it.second[s]->value);
              }
            }
            for (auto part_it2 = part2.begin(); part_it2 != part2.end(); ++part_it2) {
              bool cycle = false;
              DisjointSet<int> disjoint_set_copy = disjoint_set;
              const auto& distribution2 = part_it2.distribution();
              for (auto distr_it: distribution2) {
                for (int s=1; s<distr_it.second.size(); s++) {
                  if (disjoint_set_copy.find(distr_it.second[0]->value) ==
                      disjoint_set_copy.find(distr_it.second[s]->value)) {
                        cycle = true;
                        break;
                      }
                  disjoint_set_copy.join(distr_it.second[0]->value, distr_it.second[s]->value);
                }
              }
              if (!cycle) {
                bool equal = true;
                // Check whether x,y in the same part in pset => find(x) == find(y)
                auto pset_distribution = it.distribution();
                for (const auto& map_it : pset_distribution) {
                  for (int s = 1; s < map_it.second.size(); s++) {
                    if (disjoint_set_copy.find(map_it.second[0]->value) ==
                        disjoint_set_copy.find(map_it.second[s]->value)) continue;
                    equal = false;
                  }
                }
                // Check whether find(x) == find(y) => x and y in the same part in pset.
                map<DisjointSet<int>::Representative, vector<Node*>> merged_parts;
                for (const auto& s_m_it : sset_materialize) {
                  merged_parts[disjoint_set_copy.find(s_m_it->value)].push_back(s_m_it);
                }
                for (const auto& merged_parts_it: merged_parts) {
                  for (int s = 1; s < merged_parts_it.second.size(); s++) {
                    Node* el1 = merged_parts_it.second[0];
                    Node* el2 = merged_parts_it.second[s];
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
          printf("MERGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
                 vec[bag->id][sset.hash()][it.hash()]);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          // If we cannot take this edge to solution because
          // one of its endpoints is not in the current subset.
          if (!sset.is_present(bag->introduced_edge.first) || !sset.is_present(bag->introduced_edge.second)) {
            printf("NIE SPELNIA WARUNKOW\n");
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash()][it.hash()];
            printf("INTRODUCE_EDGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
                   vec[bag->id][sset.hash()][it.hash()]);
            break;
          }
          int part_a = it.partition(bag->introduced_edge.first);
          int part_b = it.partition(bag->introduced_edge.second);
          // If we cannot take this edge to solution because
          // its endpoints are in different partitions.
          if (part_a != part_b) {
            vec[bag->id][sset.hash()][it.hash()] = vec[bag->left->id][sset.hash()][it.hash()];
            printf("KRAWEDZ W ROZNYCH PODZBIORACH\n");
            printf("INTRODUCE_EDGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
                   vec[bag->id][sset.hash()][it.hash()]);
            break;
          }
          // When we may take the edge to the solution.
          if (!one_edge_introduced) {
            printf("we may take the edge to the solution\n");
            merge_child_partitions(bag->introduced_edge.first, bag->introduced_edge.second, vec, bag->left);
            one_edge_introduced = true;
          }
          printf("INTRODUCE_EDGE_NODE[%d][%lld][%lld] = %lld\n", bag->id, sset.hash(), it.hash(),
                 vec[bag->id][sset.hash()][it.hash()]);
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
        vec[i][j][f] = INF;
      }
    }
  }
  recursive(vec, this->tree->GetTreeWidth(), this->l, this->tree->root);
  return vec[this->tree->root->id][1][1];
}

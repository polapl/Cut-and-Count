#include "dynamic.h"

#include <cmath>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <map>

#include "tree.h"

using namespace std;
const unsigned long long int INF = 1000000;

class Set {
  public:
  class Iterator {
    public:
    void operator++() {
      for(auto& it : m_) {
        last_ = true;
        if (it.second < 2) {
          it.second++;
          last_ = false;
          break;
        }
        it.second = 0;
      }
    }

    long long int operator*() const {
      return hash();
    }

    bool operator==(const Iterator& it) const {
      return **this == *it; // not nice but works
    }

    bool operator!=(const Iterator& it) const {
      return **this != *it; // not nice but works
    }

    map<int, int> get_current_state() {
      return m_;
    }

    void set_current_state(const map<int, int>& m) {
      m_ = m;
    }

    map<int, int> give_m_() {
      return m_;
    }

    private:
      Iterator(bool last) : m_(), last_(last) {}
      map<int, int> m_;
      bool last_;
      friend class Set;
      long long int hash() const {
        if (last_) return -1;
        int hash = 0;
        int pow3 = 1;
        for(auto& it : m_) {
          hash += it.second * pow3;
          pow3 = pow3*3;
        }
        return hash;
      }
  };

  Set(const vector<Node>& nodes) : nodes_(nodes) {}

  vector<Node> nodes_;
  
  Iterator begin() {
    //cout << "begin()\n";
    if (nodes_.empty()) {
      return Iterator(true);
    }

    Iterator iterator(false);
    for (const auto& it : nodes_) {
      iterator.m_[it.value] = 0;
    }
    return iterator;
  }
  Iterator end() {
    return Iterator(true);
  }

  static Iterator from_map(const map<int, int>& m) {
    Iterator it(false);
    it.m_ = m;
    it.last_ == it.m_.size() == 0;
    return it;
  }
};

namespace {
typedef long long ll_t;
// Bag id -> number of edges -> f -> weight -> how many solutions
typedef vector<vector<vector<map<int, int>>>> dynamic_results ;
}  // namespace

void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);

  //printf("bag->id: %d\n", bag->id);

  Set set(bag->nodes);

  for(int j=0; j<=l; j++){
    if (bag->type == Bag::LEAF) {
      vec[bag->id][j][0][0] = (j == 0) ? 1 : 0;
      continue;
    }
    if (bag->type == Bag::FORGET_NODE && set.nodes_.size() == 0) {
      map<int, int> m;
      m[bag->forgotten_node.value] = 0;
      auto new_it = Set::from_map(m);
      for (auto& weight: vec[bag->left->id][j][*new_it]) {
        vec[bag->id][j][0][weight.first] += weight.second;
      }
      
      m[bag->forgotten_node.value] = 1;
      new_it.set_current_state(m);
      for (auto& weight: vec[bag->left->id][j][*new_it]) {
        vec[bag->id][j][0][weight.first] += weight.second;
      }
      
      m[bag->forgotten_node.value] = 2;
      new_it.set_current_state(m);
      for (auto& weight: vec[bag->left->id][j][*new_it]) {
        vec[bag->id][j][0][weight.first] += weight.second;
      }
      //printf("FORGET_NODE[%d][%d][0] = %d\n", bag->id, j, vec[bag->id][j][0]);
      continue;
    }
    int counter = 0;
    for(auto it = set.begin(); it != set.end(); ++it, ++counter) {
      switch(bag->type) {
        case Bag::INTRODUCE_NODE:
        {
          auto m = it.get_current_state();
          m.erase(bag->introduced_node.value);
          auto new_it = Set::from_map(m);
          for (auto& weight: vec[bag->left->id][j][*new_it]) {
            //printf("vec[%d][%d][%d][%d] += %d\n", bag->id, j, *it, weight.first, weight.second);
            //printf("vec[%d].size() = %d", bag->id, vec[bag->id].size());
            //printf("vec[%d][%d].size() = %d", bag->id, j, vec[bag->id][j].size());
            //printf("vec[%d][%d][%d].size() = %d", bag->id, j, *it, vec[bag->id][j][*it].size());
            vec[bag->id][j][*it][weight.first] += weight.second;
          }
          // terminale musza byc wziete
          if (bag->introduced_node.terminal && 
              it.get_current_state()[bag->introduced_node.value] == 0) {

            vec[bag->id][j][*it].clear();
            vec[bag->id][j][*it][0] = 0;
          }
          // v1
          if (bag->introduced_node.value == 0 &&
              it.get_current_state()[bag->introduced_node.value] != 1) {
            vec[bag->id][j][*it].clear();
            vec[bag->id][j][*it][0] = 0;
          }
          //printf("INTRODUCE_NODE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);       
          break;
        }
        case Bag::FORGET_NODE:
        {
          auto m = it.get_current_state();
          m[bag->forgotten_node.value] = 0;
          auto new_it = Set::from_map(m);
          for (auto& weight: vec[bag->left->id][j][*new_it]) {
            vec[bag->id][j][*it][weight.first] += weight.second;
          }
          
          m[bag->forgotten_node.value] = 1;
          new_it.set_current_state(m);
          for (auto& weight: vec[bag->left->id][j][*new_it]) {
            vec[bag->id][j][*it][weight.first] += weight.second;
          }
          
          m[bag->forgotten_node.value] = 2;
          new_it.set_current_state(m);
          for (auto& weight: vec[bag->left->id][j][*new_it]) {
            vec[bag->id][j][*it][weight.first] += weight.second;
          }   
          //printf("FORGET_NODE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        case Bag::MERGE:
        {
          for (int i=0; i<=j; i++) {
            for(auto& weight1: vec[bag->left->id][i][*it]) {
              for(auto& weight2: vec[bag->right->id][j-i][*it]) {
                vec[bag->id][j][*it][weight1.first + weight2.first] += weight1.second * weight2.second;
              }
            }
            
          }
          //printf("MERGE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          vec[bag->id][j][*it] = vec[bag->left->id][j][*it];
          int id_1 = it.get_current_state()[bag->introduced_edge.first.value];
          int id_2 = it.get_current_state()[bag->introduced_edge.second.value];
          if (j>0 && id_1 == id_2 && id_1 > 0) {
            for (auto& weight: vec[bag->left->id][j-1][*it]) {
              vec[bag->id][j][*it][weight.first + bag->edge_weight] += weight.second;
            }
          }
          //printf("INTRODUCE_EDGE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
      } 
    }
  }
}

unsigned long long Dynamic::Compute() {  
  /*
  dynamic_results vec = std::vector<std::vector<std::vector<std::map<int, int>>>>(this->tree->GetTreeSize() + 1);
  for(int i=0; i <= this->tree->GetTreeSize(); i++) {
    vec[i] = std::vector<std::vector<std::map<int, int>>>(this->l + 1);
    for(int j=0; j <= this->l; j++){
      vec[i][j] = std::vector<std::map<int, int>>(pow(3, this->tree->GetTreeWidth() + 2) + 1);
      for(int f=0; f <= pow(3, this->tree->GetTreeWidth() + 2); f++) {
        vec[i][j][f][0] = 0;
      }
    }
  }
  */
  int A = this->tree->GetTreeSize() + 1;
  int B = this->l + 1;
  int k = this->tree->GetTreeWidth();
  int C = pow(3, k + 1)*k + 1;
  printf("A = %d, B = %d, C = %d\n", A, B, C);
  dynamic_results vec = std::vector<std::vector<std::vector<std::map<int, int>>>>(A, std::vector<std::vector<std::map<int, int>>>(B, std::vector<std::map<int, int>>(C)));
  for(int i=0; i < A; i++) {
    for(int j=0; j < B; j++){
      for(int f=0; f < C; f++) {
        vec[i][j][f][0] = 0;
      }
    }
  }
  printf("przed recursive\n");
  recursive(vec, this->tree->GetTreeWidth(), this->l, this->tree->root);
  printf("po recursive\n");
  for(int i=0; i <= this->l; i++) {
    for (auto& weight: vec[tree->root->id][i][0]) {
      if (weight.second % 2 == 1)
        return i;
    }
  }
  return INF;
}

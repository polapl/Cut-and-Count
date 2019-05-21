#include "dynamic.h"

#include <utility>
#include <unordered_map>
#include <iostream>
#include <map>

#include "tree.h"
using namespace std;
//int int int -> int

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
          pow3++;
        }
        return hash;
      }
  };

  Set(const vector<Node*>& nodes) : nodes_(nodes) {}

  vector<Node*> nodes_;
  
  Iterator begin() {
    //cout << "begin()\n";
    if (nodes_.empty()) {
      return Iterator(true);
    }

    Iterator iterator(false);
    for (const auto& it : nodes_) {
      iterator.m_[it->value] = 0;
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
typedef vector<vector<vector<int>>> dynamic_results ;

ll_t pow(const ll_t x, const ll_t y) {
  return y ? (y & 1 ? x : 1) * pow(x * x, y >> 1) : 1;
}
}  // namespace
/*
ll_t add_to_set(ll_t set, int id, int f_id) {
  int id_to_value = pow(3, id) + f_id;
  return set + id_to_value;
}
//jesli go nie ma, to zwroc set
ll_t delete_from_set(ll_t set, int id) {
  int id_to_value = pow(3, id) * (set / pow(3, id) % 3);
  return set - id_to_value;
}

int f_id(ll_t set, int id) {
  cout << "f_id" << endl;
  return pow(3, id) + (set / pow(3, id) % 3);
}

ll_t bag_nodes_to_global_set(ll_t f) {
  int global_set = 0;
  int id = 0;
  while (f > 0) {
    int f_id = f % 3;
    global_set = add_to_set(global_set, id, f_id);
    f = f/3;
    id++;
  }
  return global_set;
}
*/

void recursive(dynamic_results &vec, int k, int l, Bag* bag) {
  if(bag == nullptr) return;
  recursive(vec, k, l, bag->left);
  recursive(vec, k, l, bag->right);

  Set set(bag->nodes);

  for(int j=0; j<=l; j++){
    int counter = 0;
    for(auto it = set.begin(); it != set.end(); ++it, ++counter) {
      //printf("%lld\n", *it);
      switch(bag->type) {
        case Bag::LEAF:
        {
          vec[bag->id][j][*it] = (j == 0) ? 1 : 0;
          printf("LEAF[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]); 
          break;
        }
        case Bag::INTRODUCE_NODE:
        {
          auto m = it.get_current_state();
          m.erase(bag->introduced_node->value);
          auto new_it = Set::from_map(m);
          vec[bag->id][j][*it] = vec[bag->left->id][j][*new_it];
          // terminale musza byc wziete
          if (bag->introduced_node->terminal && 
              it.get_current_state()[bag->introduced_node->value] == 0) {
            vec[bag->id][j][*it] = 0;
          }
          // v1
          if (bag->introduced_node->value == 0 &&
              it.get_current_state()[bag->introduced_node->value] != 1) {
            vec[bag->id][j][*it] = 0;
          }
          printf("INTRODUCE_NODE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        case Bag::FORGET_NODE:
        {
          auto m = it.get_current_state();
          m[bag->forgotten_node->value] = 0;
          auto new_it = Set::from_map(m);
          vec[bag->id][j][*it] = vec[bag->left->id][j][*new_it];
          
          m[bag->forgotten_node->value] = 1;
          new_it.set_current_state(m);
          //new_it = Set::from_map(m);
          vec[bag->id][j][*it] += vec[bag->left->id][j][*new_it];
          
          m[bag->forgotten_node->value] = 2;
          new_it.set_current_state(m);
          //new_it = Set::from_map(m);
          vec[bag->id][j][*it] += vec[bag->left->id][j][*new_it];
          
          printf("FORGET_NODE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        case Bag::MERGE:
        {
          for (int i=0; i<=j; i++) {
            vec[bag->id][j][*it] += vec[bag->left->id][i][*it] * vec[bag->right->id][j-i][*it];
          }
          printf("MERGE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          vec[bag->id][j][*it] = vec[bag->left->id][j][*it];
          int id_1 = it.get_current_state()[bag->introduced_edge.first->value];
          int id_2 = it.get_current_state()[bag->introduced_edge.second->value];
          if (j>0 && id_1 == id_2 && id_1 > 0) {
            vec[bag->id][j][*it] += vec[bag->left->id][j-1][*it];
          }
          printf("INTRODUCE_EDGE[%d][%d][%lld] = %d\n", bag->id, j, *it, vec[bag->id][j][*it]);
          break;
        }
        default:
          printf("DEFAULT\n");
      } 
    }
  }
}

int Dynamic::Compute() {
  dynamic_results vec = std::vector<std::vector<std::vector<int>>>(this->tree->GetTreeSize() + 2);
  for(int i=0; i <= this->tree->GetTreeSize(); i++) {
    vec[i] = std::vector<std::vector<int>>(this->l + 2);
    for(int j=0; j <= this->l; j++){
      vec[i][j] = std::vector<int>(pow(3, this->tree->GetTreeWidth() + 1) + 2);
      for(int f=0; f <= pow(3, this->tree->GetTreeWidth() + 1); f++) {
        vec[i][j][f] = 0;
        //printf("vec[%d][%d][%d]\n", i, j, f);
      }
    }
  }
  cout << "COMPUTE" << endl;
  recursive(vec, this->tree->GetTreeWidth(), this->l, this->tree->root);
  for(int i=0; i < this->l; i++) {
    if (vec[tree->root->id][i][0] > 0)
      return i;
  }
  return -1;
}

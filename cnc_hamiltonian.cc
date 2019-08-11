#include "cnc_hamiltonian.h"

#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

namespace {
typedef unsigned long long ull;
typedef std::unordered_map<size_t, std::unordered_map<size_t, ull>> dynamic_results;
}

void add_value(dynamic_results& vec, int a, int b, ull val) {
  if (val % 2 == 0) return;
  if (vec[a][b] % 2 == 1) {
    vec[a].erase(b);
    return;
  }
  vec[a][b] = 1;
}

void print_value(map<int, int>& col, dynamic_results& vec, int a, int b) {
	if (vec.find(a) == vec.end()) return;
	if (vec[a].find(b) == vec[a].end()) return;

    for (const auto& m: col) {
      	cout << m.first << " : " << m.second << endl;
    }

	cout << "vec[" << a << "][" << b << "] = " << vec[a][b] << endl;
}

bool in_first(int a, int b) {
	set<int> first = {0, 1};
	auto find_a = first.find(a);
	auto find_b = first.find(b);
	if (find_a == first.end()) return false;
	if (find_b == first.end()) return false;
	return true;
}

bool in_second(int a, int b) {
	set<int> second = {3, 4};
	auto find_a = second.find(a);
	auto find_b = second.find(b);
	if (find_a == second.end()) return false;
	if (find_b == second.end()) return false;
	return true;
}

const int merge_trans[5][5] = {
	{0,1,2,-1,-1},
	{1,2,-1,-1,-1},
	{2,-1,-1,-1,2},
	{-1,-1,-1,2,3},
	{-1,-1,2,3,4},
};

map<int, int> map_after_trans(map<int, int>& m1, map<int, int>& m2) {
	map<int, int> res;
	for(const auto& map_it : m1) {
		int a = map_it.second;
		int b = m2[map_it.first];
		if (merge_trans[a][b] < 0) {
			res.clear();
			return res;
		}
		res[map_it.first] = merge_trans[a][b];
	}
	return res;
}

ull hash_func(const map<int, int>& m) {
	int hash = 0;
	int pow5 = 1;
	for(auto& it : m) {
  		hash += it.second * pow5;
  		pow5 = pow5 * 5;
	}
	return hash;
}

dynamic_results merge(Bag* bag, dynamic_results& left, dynamic_results& right) {
	// cout << "MERGE " << "----------------------------------------" << endl;

	dynamic_results vec;

	State state(bag->nodes, 5);
	for(auto it1 = state.begin(); it1 != state.end(); ++it1) {
    	unsigned long long it_hash1 = *it1;

    	for(auto it2 = state.begin(); it2 != state.end(); ++it2) {
    		unsigned long long it_hash2 = *it2;
    		auto colors1 = std::move(it1.GetMapping());
    		auto colors2 = std::move(it2.GetMapping());
    		auto map_trans = map_after_trans(colors1, colors2);

    		if (map_trans.empty()) continue;

    		ull hash_trans = hash_func(map_trans);

		    for(const auto& weight1: left[it_hash1]) {
				if (weight1.second % 2 == 0) continue;
				for(const auto& weight2: right[it_hash2]) {
					add_value(vec, hash_trans, weight1.first + weight2.first, weight1.second * weight2.second);
					// print_value(map_trans, vec, hash_trans, weight1.first + weight2.first);
				}
			}
		}
    }
    return vec;
}

dynamic_results recursive_cnc_hamiltonian(Bag* bag) {
	dynamic_results vec;

	if(bag == nullptr) return vec;

	// Firstly compute partial results for subtrees.
	auto left = std::move(recursive_cnc_hamiltonian(bag->left));
	auto right = std::move(recursive_cnc_hamiltonian(bag->right));

	if(bag->type == Bag::MERGE) return merge(bag, left, right);

	/*
	switch(bag->type) {
		case Bag::INTRODUCE_NODE:
			cout << "INTRODUCE_NODE " << bag->introduced_node.value << " ----------------------------------------" << endl;
			break;
		case Bag::FORGET_NODE:
			cout << "FORGET_NODE " << bag->forgotten_node.value << " ----------------------------------------" << endl;
			break;
		case Bag::INTRODUCE_EDGE:
			cout << "INTRODUCE_EDGE " << bag->introduced_edge.first.value << " <-> " << bag->introduced_edge.second.value << " ----------------------------------------" << endl;
			break;
	}
	*/

	if (bag->type == Bag::LEAF) {
	  vec[0][0] = 1;
	  return vec;
	}

	State state(bag->nodes, 5);
	// 0,4 / 1,3 / 2 
	if (bag->type == Bag::FORGET_NODE && state.nodes_.size() == 0) {
		for (auto& weight: left[2]) {
			add_value(vec, 0, weight.first, weight.second);
	  	}
	 	return vec;
	}

    for(auto it = state.begin(); it != state.end(); ++it) {
      unsigned long long it_hash = *it;

      switch(bag->type) {
        case Bag::INTRODUCE_NODE:
        {          
        	if (it.GetMapping(bag->introduced_node.value) != 0 &&
        		it.GetMapping(bag->introduced_node.value) != 4) break;

        	if (bag->introduced_node.value == 0 &&
                it.GetMapping(bag->introduced_node.value) != 0) {
            	vec[it_hash].clear();
            	break;
          	}

			auto hash_without_node = it.GetHashWithoutNode(bag->introduced_node.value);
			for (auto& weight: left[hash_without_node]) {
				add_value(vec, it_hash, weight.first, weight.second);
				auto col = it.GetMapping();
				// print_value(col, vec, it_hash, weight.first);
			}
			break;
        }
        case Bag::FORGET_NODE:
        {
          	auto hash_with_node = it.GetAssignmentHashWithNode(bag->forgotten_node.value, 2);
          	for (auto& weight: left[hash_with_node]) {
            	add_value(vec, it_hash, weight.first, weight.second);
            	auto col = it.GetMapping();
            	// print_value(col, vec, it_hash, weight.first);
          	}
          	break;
        }
        case Bag::INTRODUCE_EDGE:
        {
          	for (auto& weight: left[it_hash]) {
              	add_value(vec, it_hash, weight.first, weight.second);
              	auto col = it.GetMapping();
              	// print_value(col, vec, it_hash, weight.first);
            }
          	int id_1 = it.GetMapping(bag->introduced_edge.first.value);
          	int id_2 = it.GetMapping(bag->introduced_edge.second.value);
          	auto hash_with_edge = it.GetAssignmentHashWithEdge(bag->introduced_edge.first.value,
          		                                               bag->introduced_edge.second.value);

          	if (in_first(id_1, id_2) || in_second(id_1, id_2)) {
            	for (auto& weight: left[it_hash]) {
              		add_value(vec, hash_with_edge, weight.first + bag->edge_weight, weight.second);
              		auto col = it.GetMapping();
              		// print_value(col, vec, hash_with_edge, weight.first + bag->edge_weight);
            	}
          	}
          	break;
        }
      } 
    }
  	return vec;
}

bool CnCHamiltonian::Compute() {  
  	dynamic_results vec = recursive_cnc_hamiltonian(this->tree->root);
  	for (auto& weight: vec[0]) {
    	if (weight.second % 2 == 1) {
        	return true;
    	}
    }
    return false;
}

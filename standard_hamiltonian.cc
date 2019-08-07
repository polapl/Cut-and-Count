#include "standard_hamiltonian.h"

#include "state_utils.h"

#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <cassert>

#define SM_HASH(a,b) (((a)<<3) | b)
#define INSERT_BLK(h,n) (((h)<<6) | n)

typedef unsigned long long ull;
typedef unordered_map<size_t, map<size_t, bool>> dynamic_results;

ull hash_c(const map<int,int>& m) {
	int hash = 0;
    int pow3 = 1;
    for(auto& it : m) {
    	//cout << "first: " << it.first << " second: " << it.second << endl;
    	hash += it.second * pow3;
    	pow3 = pow3*3;
    }
    return hash;
}

map<int, int> unhash_c(ull h) {
	map<int, int> res;
	int idx = 0;
	while(h > 0) {
		int col = h % 3;
		res[idx] = col;
		idx++;
		h /= 3;
	}
	return res;
}

ull hash_m(const map<int,int>& m) {
	ull res = 0;
	for (const auto& e : m) {
		if (e.first > e.second) {
			continue;
		}
		res = INSERT_BLK(res, SM_HASH(e.first, e.second));
	}
	return res;
}

map<int,int> unhash_m(ull h) {
	map<int,int> res;
	for (int a,b; h; h >>= 6) {
		a = h & 3, b = (h>>3) & 3;
		res[a] = b;
		res[b] = a;
	}
	return res;
}

bool get_value(const dynamic_results& vec, int idx1, int idx2) {
	auto find_res1 = vec.find(idx1);
	if (find_res1 == vec.end()) return 0;
	auto find_res2 = find_res1->second.find(idx2);
	if (find_res2 == find_res1->second.end()) return 0;
	return find_res2->second;
}

void set_value(dynamic_results& vec, int idx1, int idx2, bool val) {
  if (!val) return;
  vec[idx1][idx2] = true;
}

void print_matching(const map<int, int> m) {
	for(const auto& it : m) {
		if (it.first < it.second) {
			cout << "		" << it.first << " <-> " << it.second << endl;
		}
	}
}

void print_colors(map<int, int> colors) {
	for(const auto& debug_it : colors) {
		cout << "	" << debug_it.first << ":  " << debug_it.second << ","; 
	}
	cout << endl;
}

void print_state_for_merge(map<int, int> c_left, map<int, int> c_right, map<int, int> c_cur,
	                       map<int, int> m_left, map<int, int> m_right, map<int, int> m_cur) {
	cout << "	left: ";
	print_colors(c_left);
	cout << "	right: ";
	print_colors(c_right);
	cout << "	cur: ";
	print_colors(c_cur);

	cout << "	left: " << endl;
	print_matching(m_left);
	cout << "	right: " << endl;
	print_matching(m_right);
	cout << "	cur: " << endl;
	print_matching(m_cur);
}


dynamic_results merge_children(Bag* bag, const dynamic_results& left, const dynamic_results& right) {
	dynamic_results vec;

	State state1(bag->left->nodes, 3);
	State state2(bag->right->nodes, 3);
	// 0 + 0 = 0
	// 0 + 1 = 1*
	// 0 + 2 = 2
	// 1 + 1 = 2*

	// Iterate through all possible nodes degrees.
	// Example:
	// 0 0 0
	// 1 0 0
	// 2 0 0
	// 0 1 0
	// 0 2 0
	// 0 0 1
	// ...
	for (auto it_state1 = state1.begin(); it_state1 != state1.end(); ++it_state1) {
	  	unsigned long long it_state1_hash = *it_state1;

	  	const map<int, int>& colors1 = it_state1.give_colors(); // x:0, y:0, z:0


	  	//cout << "colors, first set: " << endl;
	  	//print_colors(colors1);

	  	// Iterate through all possible nodes degrees.
		// Example:
		// 0 0 0
		// 1 0 0
		// 2 0 0
		// 0 1 0
		// 0 2 0
		// 0 0 1
		// ...
	  	for(auto it_state2 = state2.begin(); it_state2 != state2.end(); ++it_state2) {
	  		unsigned long long it_state2_hash = *it_state2;

	  		const map<int, int>& colors2 = it_state2.give_colors(); // x:0, y:0, z:0
	  		if (colors1.size() != colors2.size()) cout << "something wrong" << endl;

	  		int ones1 = 0, ones2 = 0, ones3 = 0;

	  		map<int, int> res_color;
	  		auto it_color1 = colors1.begin();
	  		auto it_color2 = colors2.begin();

	  		bool incorrect_colors = false;

	  		for (; it_color1 != colors1.end(); it_color1++, it_color2++) {

		  		if(it_color1->second == 1) ones1++;
		  		if(it_color2->second == 1) ones2++;

				if ((it_color1->second + it_color2->second) > 2) incorrect_colors = true;
				if (incorrect_colors) break;
				if (it_color1->second + it_color2->second == 1) ones3++;
				res_color[it_color1->first] = it_color1->second + it_color2->second;
			}

	    	if (incorrect_colors) continue;
	    	if (ones1 % 2 == 1) continue;
	    	if (ones2 % 2 == 1) continue;
	    	if (ones3 % 2 == 1) continue;

	    	//cout << "colors, res: " << endl;
	    	//print_colors(res_colors);

	    	if (ones1 == 0 && ones2 == 0) {
	    		bool partial = get_value(left, it_state1_hash, 0) && get_value(right, it_state2_hash, 0);
	    		bool current = get_value(vec, hash_c(res_color), 0);
	    		set_value(vec, hash_c(res_color), 0, partial || current);
	    		
	    		// if (get_value(vec, hash_c(res_color), 0))
	    			// cout << "vec[" << hash_c(res_color) << "][0] = " << vec[hash_c(res_color)][0] << endl;
	    		continue;
	    	}
	    	if (ones1 == 0 && right.find(it_state2_hash) != right.end()) {
	    		for(const auto& it_m2 : right.at(it_state2_hash)) {
	    			bool partial = it_m2.second && get_value(left, hash_c(colors1), 0);
	    			bool current = get_value(vec, hash_c(res_color), it_m2.first);
	    			set_value(vec, hash_c(res_color), it_m2.first, partial || current);
	    		
	    			// if (get_value(vec, hash_c(res_color), it_m2.first)) {
	    				// cout << "vec[" << hash_c(res_color) << "][" << it_m2.first << "] = " << vec[hash_c(res_color)][it_m2.first] << endl;
	    				// print_state_for_merge(colors1, colors2, res_color, map<int, int>{}, unhash_m(it_m2.first), unhash_m(it_m2.first));
	    			// }
	    		}
	    		continue;
	    	}
	    	if (ones2 == 0 && left.find(it_state1_hash) != left.end()) {
	    		for(const auto& it_m1 : left.at(it_state1_hash)) {
	    			bool partial = it_m1.second && get_value(right, hash_c(colors2), 0);
	    			bool current = get_value(vec, hash_c(res_color), it_m1.first);
	    			set_value(vec, hash_c(res_color), it_m1.first, partial || current);

	    			// if (get_value(vec, hash_c(res_color), it_m1.first)) {
	    				// cout << "vec[" << hash_c(res_color) << "][" << it_m1.first << "] = " << vec[hash_c(res_color)][it_m1.first] << endl;
	    				// print_state_for_merge(colors1, colors2, res_color, unhash_m(it_m1.first), map<int, int>{}, unhash_m(it_m1.first));
	    			// }
	    		}
	    		continue;
	    	}

	    	auto all_matchings1 = it_state1.compute_matching(); // x:0, y:1, z:1, q:1 -> (0:1, 2:3), (0:2, 1:3), (0:3, 1:2)
			auto all_matchings2 = it_state2.compute_matching();
			
			for(const auto& it_m1 : all_matchings1) { // one matching : (0:1, 2:3) -> 0:1, 1:0, 2:3, 3:2	

				for(const auto& it_m2 : all_matchings2) {

					map<int, int> res_matching;
					for (const auto& path : it_m1) {
						res_matching[path.first] = path.second;
						res_matching[path.second] = path.first;
					}

					bool cycle = false;

					for (const auto& path : it_m2) {
						if (path.first > path.second) continue;

						auto f1 = res_matching.find(path.first);
						auto f2 = res_matching.find(path.second);

						if (f1 != res_matching.end() && f2 != res_matching.end()) {
							if ((*f1).second == (*f2).first) {
								cycle = true;
								break;
							} 
							res_matching[(*f1).second] = (*f2).second;
							res_matching[(*f2).second] = (*f1).second;
							res_matching.erase((*f1).first);
							res_matching.erase((*f2).first); 
							continue;
						}

						if (f1 != res_matching.end()) {
							int second_end = (*f1).second;
							res_matching.erase(f1);
							res_matching[second_end] = path.second;
							res_matching[path.second] = second_end;
							continue;
						}
						if (f2 != res_matching.end()) {
							int second_end = (*f2).second;
							res_matching.erase(f2);
							res_matching[second_end] = path.first;
							res_matching[path.first] = second_end;
							continue;
						}

						res_matching[path.first] = path.second;
						res_matching[path.second] = path.first;
					}

					if (cycle) continue;

					bool partial = get_value(left, it_state1_hash, hash_m(it_m1)) && get_value(right, it_state2_hash, hash_m(it_m2));
					bool current = get_value(vec, hash_c(res_color), hash_m(res_matching));
					set_value(vec, hash_c(res_color), hash_m(res_matching), partial || current);

			  		if (get_value(vec, hash_c(res_color), hash_m(res_matching))) {
			  			// cout << "vec[" << hash_c(res_color) << "][" << hash_m(res_matching) << "] = " << vec[hash_c(res_color)][hash_m(res_matching)] << endl;
			  			// print_state_for_merge(colors1, colors2, res_color, it_m1, it_m2, res_matching);
			  		}
			  	}
			  	
			}
		}
	}
	return vec;
}

void print_state(map<int, int> c_prev, map<int, int> c_cur, map<int, int> m_prev, map<int, int> m_cur) {
	cout << "	prev: ";
	print_colors(c_prev);
	cout << "	cur: ";
	print_colors(c_cur);
	cout << "	prev: " << endl;
	print_matching(m_prev);
	cout << "	cur: " << endl;
	print_matching(m_cur);
}

void print_all_matchings(set<map<int, int>> matchings) {
	cout << "all matchings:" << endl;
	for(const auto& m : matchings) {
		for (const auto& path : m) {
			cout << "  " << path.first << " <-> " << path.second << "  ";
		}
		cout << endl;
	}
	cout << "end" << endl;
}

bool is_edge_endpoint(int x, pair<Node, Node> edge) {
	if (x == edge.first.value) return true;
	if (x == edge.second.value) return true;
	return false;
}

dynamic_results add_edge(Bag* bag, const dynamic_results& left) {
	dynamic_results vec;

	State state(bag->left->nodes, 3);
	auto edge_idx = state.edge_idx(bag->introduced_edge.first, bag->introduced_edge.second);
	if (edge_idx.first > edge_idx.second) swap(edge_idx.first, edge_idx.second);
	// cout << "edge indexes " << edge_idx.first << " & " << edge_idx.second << endl;

	for (auto it_state = state.begin(); it_state != state.end(); ++it_state) {
	  	unsigned long long it_state_hash = *it_state;

	  	if (left.find(it_state_hash) != left.end()) {
		  	for (const auto& prev_m : left.at(it_state_hash)) {
		  		set_value(vec, it_state_hash, prev_m.first, prev_m.second);
		  		// cout << "vec[" << it_state_hash << "][" << prev_m.first << "] = " << vec[it_state_hash][prev_m.first] << " do not take" << endl;
		  	}
		}

	  	const map<int, int>& colors = it_state.give_colors(); // x:0, y:0, z:0

	  	map<int, int> res_color;

  		bool incorrect_colors = false;
  		int ones = 0;

  		for (const auto& color : colors) {

	  		if(color.second == 1) ones++;

			if (color.second > 1 && is_edge_endpoint(color.first, bag->introduced_edge)) {
				incorrect_colors = true;
				break;
			}
			res_color[color.first] = (is_edge_endpoint(color.first, bag->introduced_edge) ? color.second + 1 : color.second);
		}

    	if (incorrect_colors) continue;
    	if (ones % 2 == 1) continue;

		const auto& all_matchings = it_state.compute_matching();
		// print_all_matchings(all_matchings);

		for(const auto& it_m : all_matchings) {

			map<int, int> res_matching;
			res_matching[edge_idx.first] = edge_idx.second;
			res_matching[edge_idx.second] = edge_idx.first;

			bool cycle = false;
			for (const auto& path : it_m) {
				if (path.first > path.second) continue;

				auto f1 = res_matching.find(path.first);
				auto f2 = res_matching.find(path.second);

				if (f1 != res_matching.end() && f2 != res_matching.end()) {
					if ((*f1).second == (*f2).first) {
						cycle = true;
						break;
					} 
					res_matching[(*f1).second] = (*f2).second;
					res_matching[(*f2).second] = (*f1).second;
					res_matching.erase((*f1).first);
					res_matching.erase((*f2).first); 
					continue;
				}

				if (f1 != res_matching.end()) {
					int second_end = (*f1).second;
					res_matching.erase(f1);
					res_matching[second_end] = path.second;
					res_matching[path.second] = second_end;
					continue;
				}
				if (f2 != res_matching.end()) {
					int second_end = (*f2).second;
					res_matching.erase(f2);
					res_matching[second_end] = path.first;
					res_matching[path.first] = second_end;
					continue;
				}

				res_matching[path.first] = path.second;
				res_matching[path.second] = path.first;
			}

			if (cycle) continue;

			bool partial = get_value(left, it_state_hash, hash_m(it_m));
			bool current = get_value(vec, hash_c(res_color), hash_m(res_matching));
			set_value(vec, hash_c(res_color), hash_m(res_matching), partial || current);

	  		// cout << "vec[" << hash_c(res_color) << "][" << hash_m(res_matching) << "] = " << vec[hash_c(res_color)][hash_m(res_matching)] << endl;
			// print_state(colors, res_color, it_m, res_matching);
	  	}
	}
	return vec;
}

dynamic_results forget_node(Bag* bag, const dynamic_results& left) {
	dynamic_results vec;
	State state(bag->left->nodes, 3);
	for(auto it = state.begin(); it != state.end(); ++it) { // different colors: x:1, y:2, z:3

		if (it.get_color(bag->forgotten_node.value) != 2) continue;
		auto c_wo_n = it.c_without_node(bag->forgotten_node.value);
		auto hash_c_wo_n = hash_c(c_wo_n);

		set<int> ones = it.give_all_ones_idx();
		if(ones.size() % 2 == 1) continue;
		
		bool current = get_value(vec, hash_c_wo_n, 0);
		bool partial = get_value(left, *it, 0);

		set_value(vec, hash_c_wo_n, 0, current || partial); 
		// cout << "vec[" << hash_c_wo_n << "][0] = " << vec[hash_c_wo_n][0] << endl;
		// print_state(it.give_colors(), c_wo_n, map<int, int>(), map<int, int>());
		
		if(ones.size() == 0) continue;

		const auto& all_matchings = it.compute_matching(ones);
		// print_all_matchings(all_matchings);

		for(const auto& m : all_matchings) {
			auto m_wo_n = state.m_without_node(m, bag->forgotten_node.value);

			bool partial = get_value(left, *it, hash_m(m));
			bool current = get_value(vec, hash_c_wo_n, hash_m(m_wo_n));
			set_value(vec, hash_c_wo_n, hash_m(m_wo_n), current || partial);

			// cout << "vec[" << hash_c_wo_n << "][" << hash_m(m_wo_n) << "] = " << vec[hash_c_wo_n][hash_m(m_wo_n)] << endl;
			// print_state(it.give_colors(), c_wo_n, m, m_wo_n);
		}
	}
	return vec;
}

dynamic_results add_node(Bag* bag, const dynamic_results& left) {
	dynamic_results vec;
	State state(bag->nodes, 3);
	for(auto it = state.begin(); it != state.end(); ++it) { // different colors: x:1, y:2, z:3

		if (it.get_color(bag->introduced_node.value) != 0) continue;
		//auto hash_c_wo_n = it.hash_c_without_node(bag->introduced_node.value); // opisuje rodzica
		auto c_wo_n = it.c_without_node(bag->introduced_node.value);
		auto hash_c_wo_n = hash_c(c_wo_n);

		set<int> ones = it.give_all_ones_idx();
		if(ones.size() % 2 == 1) continue;
		
		bool current = get_value(vec, *it, 0);
		bool partial = get_value(left, hash_c_wo_n, 0);
		set_value(vec, *it, 0, current || partial);

		// cout << "vec[" << *it << "][0] = " << vec[*it][0] << endl;
		// print_state(c_wo_n, it.give_colors(), map<int, int>(), map<int, int>());
		if(ones.size() == 0) continue;
		
		const auto& all_matchings = it.compute_matching(ones);
		
		for(const auto& m : all_matchings) {
			auto m_wo_n = state.m_without_node(m, bag->introduced_node.value);

			bool current = get_value(vec, *it, hash_m(m));
			bool partial = get_value(left, hash_c_wo_n, hash_m(m_wo_n));

			set_value(vec, *it, hash_m(m), current || partial); 
			
			// cout << "vec[" << *it << "][" << hash_m(m) << "] = " << vec[*it][hash_m(m)] << endl;
			// print_state(c_wo_n, it.give_colors(), m_wo_n, m);
		}
	}
	return vec;
}

dynamic_results recursive_standard_hamiltonian(Bag* bag) {
	dynamic_results vec;

	if(bag == nullptr) return vec;

	if (bag->type == Bag::LEAF) {
		vec[0][0] = true;
		return vec;
	}

	// Firstly compute partial results for subtrees.
	auto left = std::move(recursive_standard_hamiltonian(bag->left));
	auto right = std::move(recursive_standard_hamiltonian(bag->right));

	if (bag->type == Bag::MERGE) {
		vec = merge_children(bag, left, right);
		return vec;
	}

	if (bag->type == Bag::INTRODUCE_EDGE) {
		vec = add_edge(bag, left);
		return vec;
	}

	if (bag->type == Bag::FORGET_NODE) {
		vec = forget_node(bag, left);
		return vec;
	}

	if (bag->type == Bag::INTRODUCE_NODE) {
		vec = add_node(bag, left);
		return vec;
	}
}

bool StandardHamiltonian::Compute() {  
  dynamic_results vec = recursive_standard_hamiltonian(this->tree->root);
  if (vec[4][1]) return true;
  return false;
}

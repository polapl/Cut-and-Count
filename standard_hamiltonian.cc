#include "standard_hamiltonian.h"

#include <iostream>
#include <set>
#include <map>
#include <unordered_map>

#define SM_HASH(a,b) (((a)<<3) | b)
#define INSERT_BLK(h,n) (((h)<<6) | n)

typedef unsigned long long ull;
typedef unordered_map<size_t, map<size_t, unsigned long long>> dynamic_results;

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

int get_value(const dynamic_results& vec, int idx1, int idx2) {
	auto find_res1 = vec.find(idx1);
	if (find_res1 == vec.end()) return 0;
	auto find_res2 = find_res1->second.find(idx2);
	if (find_res2 == find_res1->second.end()) return 0;
	return find_res2->second;
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

	SSet set1(bag->left->nodes);
	SSet set2(bag->right->nodes);
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
	for (auto it_set1 = set1.begin(); it_set1 != set1.end(); ++it_set1) {
	  	unsigned long long it_set1_hash = *it_set1;

	  	const map<int, int>& colors1 = it_set1.give_colors(); // x:0, y:0, z:0


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
	  	for(auto it_set2 = set2.begin(); it_set2 != set2.end(); ++it_set2) {
	  		unsigned long long it_set2_hash = *it_set2;

	  		const map<int, int>& colors2 = it_set2.give_colors(); // x:0, y:0, z:0
	  		if (colors1.size() != colors2.size()) cout << "something wrong" << endl;

	  		//cout << "colors, second set: " << endl;
	  		//print_colors(colors2);

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

	    	// CO JESLI DOKLADNIE JEDNO NIE MA MATCHINGU ?!?!
	    	if (ones1 == 0 && ones2 == 0) {
	    		vec[hash_c(res_color)][0] = get_value(left, it_set1_hash, 0) * get_value(right, it_set2_hash, 0);
	    		cout << "vec[" << hash_c(res_color) << "][0] = " << vec[hash_c(res_color)][0] << endl;
	    		print_state_for_merge(colors1, colors2, res_color, map<int, int>(), map<int, int>(), map<int, int>());
	    		continue;
	    	}

	    	const auto& all_matchings1 = it_set1.compute_matching(); // x:0, y:1, z:1, q:1 -> (0:1, 2:3), (0:2, 1:3), (0:3, 1:2)
			for(const auto& it_m1 : all_matchings1) { // one matching : (0:1, 2:3) -> 0:1, 1:0, 2:3, 3:2

				map<int, int> res_matching;
				for (const auto& path : it_m1) {
					res_matching[path.first] = path.second;
					res_matching[path.second] = path.first;
				}

				const auto& all_matchings2 = it_set2.compute_matching();
				for(const auto& it_m2 : all_matchings2) {
					bool cycle = false;
					for (const auto& path : it_m2) {
						if (path.first > path.second) continue;

						auto f1 = res_matching.find(path.first);
						auto f2 = res_matching.find(path.second);

						//f1.first jest z f1.second
						//f2.first jest z f2.second

						// to jest zle
						if (f1 != res_matching.end() && f2 != res_matching.end()) {
							if ((*f1).second == (*f2).first) {
								cycle = true;
							} else {
								// usun f1 i f2
								//...
							}
						}

						if (cycle) break;

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

			  		vec[hash_c(res_color)][hash_m(res_matching)] = get_value(left, it_set1_hash, hash_m(it_m1)) * get_value(right, it_set2_hash, hash_m(it_m2));
			  		cout << "vec[" << hash_c(res_color) << "][" << hash_m(res_matching) << "] = " << vec[hash_c(res_color)][hash_m(res_matching)] << endl;
			  		print_state_for_merge(colors1, colors2, res_color, it_m1, it_m2, res_matching);
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

dynamic_results add_edge(Bag* bag, const dynamic_results& left) {
	
	dynamic_results vec;
	SSet sset(bag->nodes);

	// Edge indexes
	auto edge_idx = sset.edge_idx(bag->introduced_edge.first, bag->introduced_edge.second);
	if (edge_idx.first > edge_idx.second) swap(edge_idx.first, edge_idx.second);
	cout << "edge indexes " << edge_idx.first << " & " << edge_idx.second << endl;

	for(auto it = sset.begin(); it != sset.end(); ++it) { // different colors: x:1, y:2, z:3
	  	unsigned long long it_hash = *it;

	  	auto colors = it.give_colors();

	  	map<int, int> res_color;
	  	bool incorrect = false;

	  	//nie bierz krawedzi

	  	for(const auto& c : colors) {
	  		res_color[c.first] = c.second;
	  		if (c.first == bag->introduced_edge.first.value) res_color[c.first]++;
	  		if (c.first == bag->introduced_edge.second.value) res_color[c.first]++;
	  		if (res_color[c.first] > 2) {
	  			incorrect = true;
	  			break;
	  		}
	  	}

		set<int> ones = it.give_all_ones_idx();
    	if(ones.size() % 2 == 1) continue;

    	if(ones.size() == 0) {

    		vec[it_hash][0] = get_value(vec, it_hash, 0) || get_value(left, it_hash, 0);
    		cout << "vec[" << it_hash << "][0] = " << vec[it_hash][0] << " do not take" << endl;
    		print_state(colors, colors, map<int, int>(), map<int, int>());

    		map<int, int> res_matching;
    		res_matching[edge_idx.first] = edge_idx.second;
    		res_matching[edge_idx.second] = edge_idx.first;
    		vec[hash_c(res_color)][hash_m(res_matching)] = get_value(vec, hash_c(res_color), hash_m(res_matching)) || get_value(left, it_hash, 0);
    		cout << "vec[" << hash_c(res_color) << "][" << hash_m(res_matching) << "] = " << vec[hash_c(res_color)][hash_m(res_matching)] << endl;
    		print_state(colors, res_color, map<int, int>(), res_matching);
    		continue; 
    	}

    	const auto& all_matchings = it.compute_matching(ones);
		
		for(const auto& m : all_matchings) {

			// Do not take an edge.
			vec[it_hash][hash_m(m)] = (get_value(vec, it_hash, hash_m(m)) || get_value(left, it_hash, hash_m(m)));
			cout << "vec[" << it_hash << "][" << hash_m(m) << "] = " << vec[it_hash][hash_m(m)] << " do not take" << endl;
			print_state(colors, colors, m, m);

			if(incorrect) continue;

			bool cycle = false;
			map<int, int> res_matching = m;

			// CZY SIE TO NIE POPSUJE SKORO MAMY a <-> b && b <-> a ?!?!
			for(const auto& path: m) {
				if (path.first > path.second) continue;
				//cout << "	" << path.first << " <-> " << path.second << endl;

				if (path.first == edge_idx.first && path.second == edge_idx.second) {
					//cout << "	 -> cycle" << endl;
					cycle = true;
					break;
				}
				if (path.first == edge_idx.first) {
					int second_endpoint = m.at(path.first);
					res_matching[second_endpoint] = edge_idx.second;
					res_matching[edge_idx.second] = second_endpoint;

					res_matching.erase(path.first);

					//cout << "	 -> " << second_endpoint << " <-> " << edge_idx.second << endl;
					break;
				}
				if (path.second == edge_idx.second) {
					int second_endpoint = m.at(path.second);
					res_matching[second_endpoint] = edge_idx.first;
					res_matching[edge_idx.first] = second_endpoint;

					res_matching.erase(path.second);

					//cout << "	 -> " << second_endpoint << " <-> " << edge_idx.first << endl;
					break;
				}
				if (path.first == edge_idx.second) {
					int second_endpoint = m.at(path.first);
					res_matching[second_endpoint] = edge_idx.first;
					res_matching[edge_idx.first] = second_endpoint;

					res_matching.erase(path.first);

					//cout << "	 -> " << second_endpoint << " <-> " << edge_idx.first << endl;
					break;
				}
				if (path.second == edge_idx.first) {
					int second_endpoint = m.at(path.second);
					res_matching[second_endpoint] = edge_idx.second;
					res_matching[edge_idx.second] = second_endpoint;

					res_matching.erase(path.second);

					//cout << "	 -> " << second_endpoint << " <-> " << edge_idx.second << endl;
					break;
				}

				//cout << "	 -> ok" << endl;
			}
			if (cycle) continue;

			if (res_matching.find(edge_idx.first) == res_matching.end() && res_matching.find(edge_idx.second) == res_matching.end()) {
				res_matching[edge_idx.first] = edge_idx.second;
    			res_matching[edge_idx.second] = edge_idx.first;
			}

			vec[hash_c(res_color)][hash_m(res_matching)] = get_value(vec, hash_c(res_color), hash_m(res_matching)) || get_value(left, it_hash, hash_m(m));
			cout << "vec[" << hash_c(res_color) << "][" << hash_m(res_matching) << "] = " << vec[hash_c(res_color)][hash_m(res_matching)] << endl;
			print_state(colors, res_color, m, res_matching);
		}
	}
	return vec;
}

void print_all_matchings(set<map<int, int>> matchings) {
	for(const auto& m : matchings) {
		for (const auto& path : m) {
			cout << "  " << path.first << " <-> " << path.second << "  ";
		}
		cout << endl;
	}
}

dynamic_results forget_node(Bag* bag, const dynamic_results& left) {
	dynamic_results vec;
	SSet sset(bag->left->nodes);
	for(auto it = sset.begin(); it != sset.end(); ++it) { // different colors: x:1, y:2, z:3

		if (it.get_color(bag->forgotten_node.value) != 2) continue;
		//auto hash_c_wo_n = it.hash_c_without_node(bag->forgotten_node.value); // opisuje rodzica
		auto c_wo_n = it.c_without_node(bag->forgotten_node.value);
		auto hash_c_wo_n = hash_c(c_wo_n);

		set<int> ones = it.give_all_ones_idx();
		if(ones.size() % 2 == 1) continue;
		
		vec[hash_c_wo_n][0] = get_value(left, *it, 0);
		cout << "vec[" << hash_c_wo_n << "][0] = " << vec[hash_c_wo_n][0] << endl;
		print_state(it.give_colors(), c_wo_n, map<int, int>(), map<int, int>());
		if(ones.size() == 0) continue;

		const auto& all_matchings = it.compute_matching(ones);
		print_all_matchings(all_matchings);

		for(const auto& m : all_matchings) {
			auto m_wo_n = sset.m_without_node(m, bag->forgotten_node.value);
			vec[hash_c_wo_n][hash_m(m_wo_n)] = vec[hash_c_wo_n][hash_m(m_wo_n)] || get_value(left, *it, hash_m(m));
			cout << "vec[" << hash_c_wo_n << "][" << hash_m(m_wo_n) << "] = " << vec[hash_c_wo_n][hash_m(m_wo_n)] << endl;
			print_state(it.give_colors(), c_wo_n, m, m_wo_n);
		}
	}
	return vec;
}

dynamic_results add_node(Bag* bag, const dynamic_results& left) {
	dynamic_results vec;
	SSet sset(bag->nodes);
	for(auto it = sset.begin(); it != sset.end(); ++it) { // different colors: x:1, y:2, z:3

		if (it.get_color(bag->introduced_node.value) != 0) continue;
		//auto hash_c_wo_n = it.hash_c_without_node(bag->introduced_node.value); // opisuje rodzica
		auto c_wo_n = it.c_without_node(bag->introduced_node.value);
		auto hash_c_wo_n = hash_c(c_wo_n);

		set<int> ones = it.give_all_ones_idx();
		if(ones.size() % 2 == 1) continue;
		
		vec[*it][0] = get_value(left, hash_c_wo_n, 0);
		cout << "vec[" << *it << "][0] = " << vec[*it][0] << endl;
		print_state(c_wo_n, it.give_colors(), map<int, int>(), map<int, int>());
		if(ones.size() == 0) continue;
		
		const auto& all_matchings = it.compute_matching(ones);
		
		for(const auto& m : all_matchings) {
			auto m_wo_n = sset.m_without_node(m, bag->introduced_node.value);

			vec[*it][hash_m(m)] = vec[*it][hash_m(m)] || get_value(left, hash_c_wo_n, hash_m(m_wo_n));
			cout << "vec[" << *it << "][" << hash_m(m) << "] = " << vec[*it][hash_m(m)] << endl;
			print_state(c_wo_n, it.give_colors(), m_wo_n, m);
		}
	}
	return vec;
}

dynamic_results recursive(Bag* bag) {
	dynamic_results vec;

	if(bag == nullptr) return vec;

	if (bag->type == Bag::LEAF) {
		vec[0][0] = true;
		return vec;
	}

	// Firstly compute partial results for subtrees.
	auto left = std::move(recursive(bag->left));
	auto right = std::move(recursive(bag->right));

	if (bag->type == Bag::MERGE) {
		for(int i=10; i>0; i--) cout << "-----";
		cout << "MERGE" << endl;
		vec = merge_children(bag, left, right);
		cout << endl;
		return vec;
	}

	if (bag->type == Bag::INTRODUCE_EDGE) {
		for(int i=10; i>0; i--) cout << "-----";
		cout << "INTRODUCE_EDGE " << bag->introduced_edge.first.value << " " << bag->introduced_edge.second.value << endl;
		vec = add_edge(bag, left);
		cout << endl;
		return vec;
	}

	if (bag->type == Bag::FORGET_NODE) {
		for(int i=10; i>0; i--) cout << "-----";
		cout << "FORGET_NODE : " << bag->forgotten_node.value << endl;
		vec = forget_node(bag, left);
		return vec;
	}

	if (bag->type == Bag::INTRODUCE_NODE) {
		for(int i=10; i>0; i--) cout << "-----";
		cout << "INTRODUCE_NODE : " << bag->introduced_node.value << endl;
		vec = add_node(bag, left);
		return vec;
	}
}

bool StandardHamiltonian::Compute() {  
  dynamic_results vec = recursive(this->tree->root);
  if (vec[4][1]) return true;
  return false;
}

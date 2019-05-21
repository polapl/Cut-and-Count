#include "tree.h"
#include <cstdlib>
#include <cassert>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <iomanip> 

using namespace std;

int bag_id = 0;

int get_next_id() {
    return bag_id++;
}

template<class T>
void push_vec(queue<T>& Q, const vector<T>& v) {
    for (const auto& x : v) {
        Q.push(x); 
    } 
}

int max_node = 0;

Node* nextNode(int probability) {
    bool terminal = (rand() % 100 < probability) ? true : false;
    return new Node(max_node++, terminal);
}

Bag::Bag(Bag::BagType type, Bag* parent, Bag** ref, const vector<Node*>& nodes) : type(type), id(get_next_id()), parent(parent), nodes(nodes), left(nullptr), right(nullptr)  {
    if (parent == nullptr) {
        rank = 0;
    }
    else {
        rank= parent->rank + 1;
        *ref = this;
    }
}

vector<Bag*> Bag::Generate(const vector<Bag::BagType>& types, int probability) {
    vector<Bag*> bags;
    switch(this->type) {
        case Bag::BagType::FORGET_NODE:
        {
            vector<Node*> nodes = this->nodes;
            Node* forgotten_node = nextNode(probability);
            nodes.push_back(forgotten_node);
            Bag* b = new Bag(types[0], this, &this->left, nodes);
            this->forgotten_node = forgotten_node;
            bags.push_back(b);
            break;
        }
        case Bag::BagType::INTRODUCE_NODE:
        {
            vector<Node*> nodes = this->nodes;
            int r = rand() % (nodes.size());
            Node* introduced_node = nodes[r];
            nodes.erase(nodes.begin() + r);
            Bag* b = new Bag(types[0], this, &this->left, nodes);
            this->introduced_node = introduced_node;
            bags.push_back(b);
            break;
        }
        case Bag::BagType::MERGE:
        {
            assert(types.size() == 2);
            Bag* b1 = new Bag(types[0], this, &this->left, this->nodes);
            Bag* b2 = new Bag(types[1], this, &this->right, this->nodes);
            bags.push_back(b1);
            bags.push_back(b2);
            break;
        }
        case Bag::BagType::LEAF:
            cout << "Leaf\n";
        default:
            cout << "Default\n";
    }
    return bags;
}

void Tree::Generate(int size, int probability) {
    Bag* root = new Bag(Bag::BagType::FORGET_NODE, nullptr, nullptr, vector<Node*>{});
    this->root = root;
    Bag* parent = root;
    queue<Bag*> Q;

    for (int i=0; i<5; i++) {
        vector<Bag*> bags = parent->Generate(vector<Bag::BagType> {Bag::BagType::FORGET_NODE}, probability);
        parent = bags[0];
    }
    Q.push(parent);
    vector<Bag::BagType> types = {
        Bag::BagType::INTRODUCE_NODE,
        Bag::BagType::MERGE,
        Bag::BagType::FORGET_NODE,
    };
    while (!Q.empty() && size >= 0) {
        Bag* front = Q.front();
        Q.pop();
        if (front == nullptr) continue;
        if (front->nodes.size() == 0 && front != this->root) {
            front->type = Bag::BagType::LEAF;
            front->parent->introduced_node = (front->parent->nodes)[0];
            continue;
        }
        if (front->type == Bag::BagType::MERGE) {
            int r1, r2;
            if (front->nodes.size() == this->tree_width + 1) {
                r1 = rand() % 2;
                r2 = rand() % 2;
            } else {
                r1 = rand() % 3;
                r2 = rand() % 3;
            }
            Bag::BagType type1 = types[r1];
            Bag::BagType type2 = types[r2];
            vector<Bag*> bags = front->Generate({type1, type2}, probability);
            push_vec(Q, bags);
            size -= 2;
            continue;
        } 
        int r;
        if (front->nodes.size() == this->tree_width + 1) {
            r = rand() % 2;
        } else {
            r = rand() % 3; 
        }
        Bag::BagType type = types[r];
        vector<Bag*> bags = front->Generate({type}, probability);
        push_vec(Q, bags);
        size--;
    }
    while (!Q.empty()) {
        Bag* front = Q.front();
        Q.pop();
        if (front == nullptr) continue;
        if (front->nodes.size() == 0 && front != this->root) {
            front->type = Bag::BagType::LEAF;
            continue;
        }
        vector<Bag*> bags = front->Generate({Bag::BagType::INTRODUCE_NODE, Bag::BagType::INTRODUCE_NODE}, probability);
        push_vec(Q, bags);
    }
}

void Dfs(Bag* bag, int probability, int max_weight) {
    if (bag == nullptr) return;
    if (bag->type == Bag::BagType::FORGET_NODE) {
        assert(bag->forgotten_node);
        Bag* bag_it = bag;
        for (auto& x : bag->nodes) {
            int r = rand() % 100;
            if (r < probability) {
                // There is an edge: x -- forgotten_node
                Bag* current_left = bag_it->left;
                Bag* new_bag = new Bag(Bag::BagType::INTRODUCE_EDGE, bag_it, &bag_it->left, bag_it->left->nodes);
                new_bag->left = current_left;
                current_left->parent = new_bag;

                new_bag->introduced_edge = make_pair(x, bag_it->forgotten_node);
                new_bag->edge_weight = rand() % max_weight;
            }
        }
    }
    Dfs(bag->left, probability, max_weight);
    Dfs(bag->right, probability, max_weight);
}

void Tree::IntroduceEdges(int probability) {
    Dfs(this->root, probability, this->max_weight);
}

void Tree::DotTransitionGraph(string file_path) {
    map<int, vector<int>> rank;
    ofstream myfile;
    myfile.open(file_path);
    myfile << "graph G {" << endl;
    myfile << "ratio = fill;" << endl;
    myfile << "size = \"16, 9\";" << endl;;
    myfile << "node [fontsize = 300, fontname=\"Courier-Bold\"];" << endl;
    myfile << "edge [penwidth = 25, layer=\"background\", arrowhead=vee, arrowsize=25.0];" << endl;
    queue<Bag*> Q;
    Q.push(this->root);
    this->root->Print();
    while (!Q.empty()) {
        Bag* front = Q.front();
        Q.pop();
        if (front == nullptr) continue;
        if (front -> left) {
            Q.push(front->left);
        } 
        if (front->right) {
            Q.push(front->right);
        }
        cout << front->id << endl;
        rank[front->rank].push_back(front->id);
        myfile << front->id << " [label= \"" << front->Label() << "]" << endl;
        if (front->parent != nullptr) {
            myfile << front->parent->id << " -- " << front->id << ";" << endl;
        }
    }
    cout << "po forze" << endl;

    myfile << "}\n";
    myfile.close();
}


    string Bag::ToString() const {
        stringstream sstr;
        #define HEX(num) std::hex << setfill('0') << setw(5) <<  (0xfffff & num) << std::dec
        #define ADR(ptr) HEX(reinterpret_cast<unsigned long long>(ptr))
        sstr << "[" << ADR(this) << "] - ";
        sstr << "id: " << setw(5) << id << " | ";
        sstr << "rank: " << setw(5) << rank << " | ";
        sstr << "left: " << "[" << ADR(left) << "] | ";
        sstr << "right: " << "[" << ADR(right) << "] | ";
        sstr << "parent: " << "[" << ADR(parent) << "]";
        sstr << endl;

        return sstr.str();
    }

    void Bag::Print() const {
        cout << this->ToString() << endl;
        if (this->left) {
            this->left->Print();
        } 
        if (this->right) {
            this->right->Print();
        } 
    }

    string Bag::Label() const {
        string type, color, additional_info = "";
        switch(this->type) {
            case Bag::BagType::INTRODUCE_NODE:
                type = "INTRODUCE_NODE";
                color = "aquamarine";
                additional_info = to_string(this->introduced_node->value);
                break;
            case Bag::BagType::INTRODUCE_EDGE:
                type = "INTRODUCE_EDGE";
                color = "cornflowerblue";
                additional_info = to_string(this->introduced_edge.first->value);
                additional_info += to_string(this->introduced_edge.second->value);
                break;
            case Bag::BagType::FORGET_NODE:
                type = "FORGET_NODE";
                color = "cyan4";
                additional_info = to_string(this->forgotten_node->value);
                break;
            case Bag::BagType::MERGE:
                type = "MERGE";
                color = "orange1";
                break;
            case Bag::BagType::LEAF:
                type = "LEAF";
                color = "darkolivegreen2";
                break;
        }
        stringstream label;
        label << type << " " << additional_info << " [id: " << this->id << "] : { ";
        for (const auto& x : this->nodes) {
            label << x->value << " , ";
        }
        label << "}\", color=" << color << ", style=filled";
        return label.str();
    }

int Tree::GetGraphSize() {
    return max_node;
}

int Tree::GetTreeSize() {
    return bag_id;
}

int Tree::GetTreeWidth() {
    return this->tree_width;
}

std::pair<Bag*, std::vector<Bag*>::iterator> generate_rec(Bag* parent, std::vector<Bag*>::iterator next_to_add) {
  Bag* current = *next_to_add;
  std::pair<Bag*, std::vector<Bag*>::iterator> rec;
  switch(current->type) {
    case Bag::LEAF:
    {
      current->parent = parent;
      return std::make_pair(current, next_to_add++);
    }
    case Bag::INTRODUCE_NODE:
    {
      current->parent = parent;
      rec = generate_rec(current, next_to_add++);
      current->left = rec.first;
      return std::make_pair(current, rec.second);
    }
    case Bag::FORGET_NODE:
    {
      current->parent = parent;
      rec = generate_rec(current, next_to_add++);
      current->left = rec.first;
      return std::make_pair(current, rec.second);
    }
    case Bag::MERGE:
    {
      current->parent = parent;
      rec = generate_rec(current, next_to_add++);
      current->left = rec.first;
      rec = generate_rec(current, rec.second);
      current->right = rec.first;
      return std::make_pair(current, rec.second);
    }
    case Bag::INTRODUCE_EDGE:
    {
      current->parent = parent;
      rec = generate_rec(current, next_to_add++);
      current->left = rec.first;
      return std::make_pair(current, rec.second);
    }
    default:
      printf("DEFAULT\n");
  } 
}

Tree* GenerateFromInput(std::vector<Bag*> &bags) {
  std::pair<Bag*, std::vector<Bag*>::iterator> rec = generate_rec(nullptr, bags.begin());
  Tree* tree = new Tree();
  tree->root = rec.first;
  return tree;
}

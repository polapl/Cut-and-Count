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

Bag::Bag(Bag::BagType type, Node* node) : type(type), id(get_next_id()) {
    if (type == Bag::BagType::INTRODUCE_NODE) {
        this->introduced_node = node;
    } 
    if (type == Bag::BagType::FORGET_NODE) {
        this->forgotten_node = node;
    }
}
Bag::Bag(Bag::BagType type, std::pair<Node*, Node*> introduced_edge, int edge_weight) : type(type), introduced_edge(introduced_edge), edge_weight(edge_weight), id(get_next_id()) {}
Bag::Bag(Bag::BagType type) : type(type), id(get_next_id()) {}

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

    vector<Bag*> bags = parent->Generate(vector<Bag::BagType> {Bag::BagType::FORGET_NODE}, 1);
    parent = bags[0];

    for (int i=0; i<4; i++) {
        bags = parent->Generate(vector<Bag::BagType> {Bag::BagType::FORGET_NODE}, probability);
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
    //this->root->Print();
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
        //cout << front->id << endl;
        rank[front->rank].push_back(front->id);
        myfile << front->id << " [label= \"" << front->Label() << "]" << endl;
        if (front->parent != nullptr) {
            myfile << front->parent->id << " -- " << front->id << ";" << endl;
        }
    }
    //cout << "po forze" << endl;

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

void Bag::print() {
    cout << "nodes:\n";
    for (auto& el: this->nodes) {
        cout << el->value << " ";
    }
    cout << "type: " << this->type << endl;
    if (this->parent) cout << "parent: " << this->parent->id << endl;
    printf("zomfg\n");
    if (this->left) cout << "left: " << this->left->id << endl;
    printf("zomfg\n");
    if (this->right) cout << "right: " << this->right->id << endl;
    printf("zomfg\n");
    if (this->introduced_node) cout << this->introduced_node << endl;
    if (this->forgotten_node) cout << this->forgotten_node << endl;
    if (this->introduced_edge.first) cout << this->introduced_edge.first->value << this->introduced_edge.second->value << endl;
}

std::pair<Bag*, std::vector<Bag*>::iterator> generate_rec(Bag* parent, std::vector<Bag*>::iterator next_to_add) {
  Bag* current = *next_to_add;
  
  std::pair<Bag*, std::vector<Bag*>::iterator> rec;
  
  switch(current->type) {
    case Bag::LEAF:
    {
      current->parent = parent;
      current->nodes = vector<Node*>{};
      current->left = rec.first;
      current->right = nullptr;
      //current->print();
      printf("cur: %d, nodes:\n", current->id);
      for (auto& it: current->nodes) {
          printf("%d ", it->value);
      }
      printf("\n");
      return std::make_pair(current, ++next_to_add);
    }
    case Bag::INTRODUCE_NODE:
    {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      current->nodes = current->left->nodes;
      current->nodes.push_back(current->introduced_node);
      //current->print();
      printf("cur: %d, nodes:\n", current->id);
      for (auto& it: current->nodes) {
          printf("%d ", it->value);
      }
      printf("\n");
      return std::make_pair(current, rec.second);
    }
    case Bag::FORGET_NODE:
    {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      for (auto& n: current->left->nodes) {
          if(n->value != current->forgotten_node->value) {
            current->nodes.push_back(n);
          }
      }
      //current->print();
      printf("cur: %d, nodes:\n", current->id);
      for (auto& it: current->nodes) {
          printf("%d ", it->value);
      }
      printf("\n");
      return std::make_pair(current, rec.second);
    }
    case Bag::MERGE:
    {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      rec = generate_rec(current, rec.second);
      current->right = rec.first;
      current->nodes = current->left->nodes;
      //current->print();
      printf("cur: %d, nodes:\n", current->id);
      for (auto& it: current->nodes) {
          printf("%d ", it->value);
      }
      printf("\n");
      return std::make_pair(current, rec.second);
    }
    case Bag::INTRODUCE_EDGE:
    {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      current->nodes = current->left->nodes;
      printf("cur: %d, nodes:\n", current->id);
      for (auto& it: current->nodes) {
          printf("%d ", it->value);
      }
      printf("\n");
      //current->print();
      return std::make_pair(current, rec.second);
    }
    default:
      printf("DEFAULT\n");
  } 
}

Tree::Tree(std::vector<Bag*> &bags) {
  std::pair<Bag*, std::vector<Bag*>::iterator> rec = generate_rec(nullptr, bags.begin());
  this->root = rec.first;
}

void Tree::AddNodeToAllBags(Bag* b, Node* n) {
    if (b == nullptr) return;
    if (b->type == Bag::INTRODUCE_NODE && b->introduced_node == n) {
        b->parent->left = b->left;
        b->left->parent = b->parent;
    }
    bool already_present = false;
    for (auto& el: b->nodes) {
        if (el == n) already_present = true;
    } 
    if (!already_present) b->nodes.push_back(n);
    AddNodeToAllBags(b->left, n);
    AddNodeToAllBags(b->right, n);
}
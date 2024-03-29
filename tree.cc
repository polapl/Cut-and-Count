#include "tree.h"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int bag_id = 0;

int get_next_id() { return bag_id++; }

template <class T>
void push_vec(queue<T>& Q, const vector<T>& v) {
  for (const auto& x : v) {
    Q.push(x);
  }
}

int max_node = 0;

Node nextNode(int probability) {
  bool terminal = (rand() % 100 < probability) ? true : false;
  return Node(max_node++, terminal);
}

Bag::Bag(Bag::BagType type, Bag* parent, Bag** ref, const vector<Node>& nodes)
    : type(type),
      id(get_next_id()),
      parent(parent),
      nodes(nodes),
      left(nullptr),
      right(nullptr) {
  if (parent == nullptr) {
    rank = 0;
  } else {
    rank = parent->rank + 1;
    *ref = this;
  }
}

Bag::Bag(Bag::BagType type, const Node& node)
    : type(type), id(get_next_id()), left(nullptr), right(nullptr) {
  if (type == Bag::BagType::INTRODUCE_NODE) {
    this->introduced_node = node;
  }
  if (type == Bag::BagType::FORGET_NODE) {
    this->forgotten_node = node;
  }
}
Bag::Bag(Bag::BagType type, const std::pair<Node, Node>& introduced_edge,
         int edge_weight = 0)
    : type(type),
      introduced_edge(introduced_edge),
      edge_weight(edge_weight),
      id(get_next_id()),
      left(nullptr),
      right(nullptr) {}
Bag::Bag(Bag::BagType type)
    : type(type), id(get_next_id()), left(nullptr), right(nullptr) {}

vector<Bag*> Bag::Generate(const vector<Bag::BagType>& types, int probability) {
  vector<Bag*> bags;
  switch (this->type) {
    case Bag::BagType::FORGET_NODE: {
      vector<Node> nodes = this->nodes;
      Node forgotten_node = nextNode(probability);
      nodes.push_back(forgotten_node);
      Bag* b = new Bag(types[0], this, &this->left, nodes);
      this->forgotten_node = forgotten_node;
      bags.push_back(b);
      break;
    }
    case Bag::BagType::INTRODUCE_NODE: {
      vector<Node> nodes = this->nodes;
      int r = rand() % (nodes.size());
      Node introduced_node = nodes[r];
      nodes.erase(nodes.begin() + r);
      Bag* b = new Bag(types[0], this, &this->left, nodes);
      this->introduced_node = introduced_node;
      bags.push_back(b);
      break;
    }
    case Bag::BagType::MERGE: {
      assert(types.size() == 2);
      Bag* b1 = new Bag(types[0], this, &this->left, this->nodes);
      Bag* b2 = new Bag(types[1], this, &this->right, this->nodes);
      bags.push_back(b1);
      bags.push_back(b2);
      break;
    }
  }
  return bags;
}

void Tree::Generate(int size, int probability) {
  bag_id = 0;
  max_node = 0;
  // Root is always of FORGET_NODE type.
  Bag* root =
      new Bag(Bag::BagType::FORGET_NODE, nullptr, nullptr, vector<Node>{});
  this->root = root;
  Bag* parent = root;
  queue<Bag*> Q;

  // First Node is always a terminal.
  vector<Bag*> bags =
      parent->Generate(vector<Bag::BagType>{Bag::BagType::FORGET_NODE}, 100);
  parent = bags[0];

  // Add as many forget Bags as possible in order to introduce new Nodes.
  for (int i = 0; i < tree_width - 1; i++) {
    bags = parent->Generate(vector<Bag::BagType>{Bag::BagType::FORGET_NODE},
                            probability);
    parent = bags[0];
  }
  Q.push(parent);
  vector<Bag::BagType> types = {
      Bag::BagType::INTRODUCE_NODE,
      Bag::BagType::MERGE,
      Bag::BagType::FORGET_NODE,
  };
  // The rest of a graph is generated randomly.
  while (!Q.empty() && size >= 0) {
    Bag* front = Q.front();
    Q.pop();
    if (front == nullptr) continue;
    if (front->nodes.size() == 0 && front != this->root) {
      front->type = Bag::BagType::LEAF;
      front->parent->introduced_node = (front->parent->nodes)[0];
      continue;
    }
    // Merge has always two children.
    if (front->type == Bag::BagType::MERGE) {
      int r1, r2;
      // If we reached tree_width and cannot introduce any new Nodes.
      if (front->nodes.size() == this->tree_width + 1 ||
          (front->nodes.size() == this->tree_width &&
           front->type == Bag::BagType::FORGET_NODE)) {
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
    // If we reached tree_width and cannot introduce any new Nodes.
    if (front->nodes.size() == this->tree_width + 1 ||
        (front->nodes.size() == this->tree_width &&
         front->type == Bag::BagType::FORGET_NODE)) {
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
    vector<Bag*> bags = front->Generate(
        {Bag::BagType::INTRODUCE_NODE, Bag::BagType::INTRODUCE_NODE},
        probability);
    push_vec(Q, bags);
  }
}

void Dfs(Bag* bag, int probability, int max_weight) {
  if (bag == nullptr) return;
  if (bag->type == Bag::BagType::FORGET_NODE) {
    Bag* bag_it = bag;
    for (auto& x : bag->nodes) {
      int r = rand() % 100;
      if (r < probability) {
        // There is an edge: x -- forgotten_node
        Bag* current_left = bag_it->left;
        Bag* new_bag = new Bag(Bag::BagType::INTRODUCE_EDGE, bag_it,
                               &bag_it->left, bag_it->left->nodes);
        new_bag->left = current_left;
        current_left->parent = new_bag;

        if (x.value < bag_it->forgotten_node.value)
          new_bag->introduced_edge = make_pair(x, bag_it->forgotten_node);
        else
          new_bag->introduced_edge = make_pair(bag_it->forgotten_node, x);
        new_bag->edge_weight = rand() % max_weight;
      } else {
        assert(true);
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
  myfile << "size = \"16, 9\";" << endl;
  ;
  myfile << "node [fontsize = 300, fontname=\"Courier-Bold\", "
            "colorscheme=\"pastel25\"];"
         << endl;
  myfile << "edge [penwidth = 25, layer=\"background\", arrowhead=vee, "
            "arrowsize=25.0];"
         << endl;
  queue<Bag*> Q;
  Q.push(this->root);
  while (!Q.empty()) {
    Bag* front = Q.front();
    Q.pop();
    if (front == nullptr) continue;
    if (front->left) {
      Q.push(front->left);
    }
    if (front->right) {
      Q.push(front->right);
    }
    rank[front->rank].push_back(front->id);
    myfile << front->id << " [label= \"" << front->Label() << "]" << endl;
    if (front->parent != nullptr) {
      myfile << front->parent->id << " -- " << front->id << ";" << endl;
    }
  }

  myfile << "}\n";
  myfile.close();
}

string Bag::Label() const {
  string type, color, additional_info = "";
  switch (this->type) {
    case Bag::BagType::INTRODUCE_NODE:
      type = "INTRODUCE_NODE";
      color = "1";
      additional_info = to_string(this->introduced_node.value);
      if (this->introduced_node.terminal) additional_info += "*";
      break;
    case Bag::BagType::INTRODUCE_EDGE:
      type = "INTRODUCE_EDGE";
      color = "2";
      additional_info = to_string(this->introduced_edge.first.value);
      additional_info += to_string(this->introduced_edge.second.value);
      break;
    case Bag::BagType::FORGET_NODE:
      type = "FORGET_NODE";
      color = "3";
      additional_info = to_string(this->forgotten_node.value);
      break;
    case Bag::BagType::MERGE:
      type = "MERGE";
      color = "4";
      break;
    case Bag::BagType::LEAF:
      type = "LEAF";
      color = "5";
      break;
  }
  stringstream label;
  label << type << " " << additional_info << " [id: " << this->id << "] : { ";
  for (const auto& x : this->nodes) {
    label << x.value << " , ";
  }
  label << "}\", color=" << color << ", style=filled";
  return label.str();
}

int Tree::GetGraphSize() { return max_node; }

int Tree::GetTreeSize() { return bag_id; }

int Tree::GetTreeWidth() { return this->tree_width; }

// This function is used only for testing.
// It generates tree based on a given vector of Bags,
// preserving their order from top (root) to bottom (leaves).
std::pair<Bag*, std::vector<Bag*>::iterator> generate_rec(
    Bag* parent, std::vector<Bag*>::iterator next_to_add) {
  Bag* current = *next_to_add;

  std::pair<Bag*, std::vector<Bag*>::iterator> rec;

  switch (current->type) {
    case Bag::LEAF: {
      current->parent = parent;
      current->nodes = vector<Node>{};
      current->left = rec.first;
      current->right = nullptr;
      return std::make_pair(current, ++next_to_add);
    }
    case Bag::INTRODUCE_NODE: {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      current->nodes = current->left->nodes;
      current->nodes.push_back(current->introduced_node);
      return std::make_pair(current, rec.second);
    }
    case Bag::FORGET_NODE: {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      for (auto& n : current->left->nodes) {
        if (n.value != current->forgotten_node.value) {
          current->nodes.push_back(n);
        }
      }
      return std::make_pair(current, rec.second);
    }
    case Bag::MERGE: {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      rec = generate_rec(current, rec.second);
      current->right = rec.first;
      current->nodes = current->left->nodes;
      return std::make_pair(current, rec.second);
    }
    case Bag::INTRODUCE_EDGE: {
      current->parent = parent;
      rec = generate_rec(current, ++next_to_add);
      current->left = rec.first;
      current->right = nullptr;
      current->nodes = current->left->nodes;
      return std::make_pair(current, rec.second);
    }
    default:
      assert(false);  // this case is not supported
  }
}

Tree::Tree(std::vector<Bag*>& bags) {
  std::pair<Bag*, std::vector<Bag*>::iterator> rec =
      generate_rec(nullptr, bags.begin());
  this->root = rec.first;
}

void Tree::AddNodeToAllBags(Bag* b, Node n, bool front) {
  if (b == nullptr) return;
  if (b->type == Bag::INTRODUCE_NODE && b->introduced_node == n) {
    if (b->parent->left == b) {
      b->parent->left = b->left;
      b->left->parent = b->parent;
    } else {
      b->parent->right = b->left;
      b->left->parent = b->parent;
    }
  }
  bool already_present = false;
  for (auto& el : b->nodes) {
    if (el == n) already_present = true;
  }
  if (!already_present && !front) b->nodes.push_back(n);
  if (!already_present && front) b->nodes.insert(b->nodes.begin(), n);
  AddNodeToAllBags(b->left, n, front);
  AddNodeToAllBags(b->right, n, front);
}

void second_root(Bag* bag, int root_val, Node root_copy, Bag** parent_ref) {
  if (bag == nullptr) return;

  bool present = false;
  for (const auto& node : bag->nodes) {
    if (node.value == root_val) {
      present = true;
      break;
    }
  }

  bool bag_introducing_root = (bag->type == Bag::BagType::INTRODUCE_NODE &&
                               bag->introduced_node.value == root_val);

  if (present && !bag_introducing_root) bag->nodes.push_back(root_copy);

  if (bag->type == Bag::BagType::INTRODUCE_EDGE &&
      bag->introduced_edge.first.value == root_val) {
    Node edge_node(bag->introduced_edge.second.value);
    Bag* new_edge =
        new Bag(Bag::BagType::INTRODUCE_EDGE, make_pair(edge_node, root_copy));
    new_edge->nodes = bag->nodes;

    *parent_ref = new_edge;
    new_edge->parent = bag->parent;
    bag->parent = new_edge;
    new_edge->left = bag;
  }

  if (bag->type == Bag::BagType::INTRODUCE_EDGE &&
      bag->introduced_edge.second.value == root_val) {
    Node edge_node(bag->introduced_edge.first.value);
    Bag* new_edge =
        new Bag(Bag::BagType::INTRODUCE_EDGE, make_pair(edge_node, root_copy));
    new_edge->nodes = bag->nodes;

    *parent_ref = new_edge;
    new_edge->parent = bag->parent;
    bag->parent = new_edge;
    new_edge->left = bag;
  }

  if (bag->type == Bag::BagType::INTRODUCE_NODE &&
      bag->introduced_node.value == root_val) {
    Bag* new_root = new Bag(Bag::BagType::INTRODUCE_NODE, root_copy);
    new_root->nodes = bag->nodes;

    *parent_ref = new_root;
    new_root->parent = bag->parent;
    bag->parent = new_root;
    new_root->left = bag;

    new_root->nodes.push_back(root_copy);
  }

  if (bag->left != nullptr)
    second_root(bag->left, root_val, root_copy, &bag->left);
  if (bag->right != nullptr)
    second_root(bag->right, root_val, root_copy, &bag->right);
}

void Tree::PrepareBeforeStandardHamiltonian(
    const Node& root_copy = nextNode(0)) {
  int root_val = this->root->forgotten_node.value;
  this->root = this->root->left;
  this->root->parent = nullptr;
  second_root(this->root, root_val, root_copy, nullptr);
}

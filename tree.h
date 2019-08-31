#pragma once

#include <algorithm>
#include <iostream>

#include <set>
#include <string>
#include <vector>
#include <utility>

using namespace std;

// Node corresponds to a graph node.
struct Node {
    int value;
    bool terminal = false;

    bool operator<(const Node& a) const {
        return this->value < a.value;
    }

    bool operator==(const Node& a) const {
        return this->value == a.value;
    }

    bool operator!=(const Node& a) const {
        return this->value != a.value;
    }

    Node() {}
    Node(int value, bool terminal = false): value(value), terminal(terminal) {}
};

// Bag corresponds to a tree node, may contain many Nodes.
class Bag {
public:
    enum BagType {
        LEAF = 0,
        INTRODUCE_NODE,
        INTRODUCE_EDGE,
        FORGET_NODE,
        MERGE,
    };

    Bag(Bag::BagType type, Bag* parent, Bag** ref, const std::vector<Node>& nodes);
    
    Bag(Bag::BagType type, const Node& introduced_node);
    Bag(Bag::BagType type, const std::pair<Node, Node>& introduced_edge, int edge_weight);
    Bag(Bag::BagType type);

    std::vector<Node> nodes;
    BagType type;
    Bag* parent;
    Bag* left;
    Bag* right;

    // Set only when bag is of INTRODUCE_NODE type.
    Node introduced_node;
    // Set only when bag is of FORGET_NODE type.
    Node forgotten_node;
    // Set only when bag is of INTRODUCE_EDGE type.
    std::pair<Node, Node> introduced_edge;
    long long int edge_weight;

    // Generates children bags based on the given types and probability.
    std::vector<Bag*> Generate(const std::vector<Bag::BagType>& types, int probability);

    std::string Label() const;

    int rank;
    int id;

};

class Tree {
public:
    Tree() {}
    Tree(int tree_width, int max_weight) : tree_width(tree_width), max_weight(max_weight) {}
    Bag* root;
    int tree_width;
    int max_weight;
    // Arg 1: number of bags whose children have randomly generated type.
    // Arg 2: probability per node it is a terminal.
    void Generate(int, int);
    // Used only for manual tests.
    // Bags should have only type and terminal set.
    Tree(std::vector<Bag*> &bags);
    // Arg: probability yes [0,100]
    void IntroduceEdges(int);
    // Visualization
    void DotTransitionGraph(std::string file_path);
    int GetGraphSize();
    int GetTreeSize();
    int GetTreeWidth();
    // Used for Steiner tree standard dynamic which assumes 
    // one terminal Node is present in all Bags.
    void AddNodeToAllBags(Bag* b, Node n, bool front);
    // Splits root into two Bags, copies all edges that are incident to root,
    // i.e. for every edge(root_1, x), function adds edge(root_2, x). 
    void PrepareBeforeStandardHamiltonian(const Node& zero_prim);
};

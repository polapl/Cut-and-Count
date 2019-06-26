#pragma once

#include <set>
#include <string>
#include <vector>
#include <utility>

struct Node {
    int value;
    bool terminal;

    Node(int value, bool terminal): value(value), terminal(terminal) {}
};

class Bag {
public:
    enum BagType {
        LEAF = 0,
        INTRODUCE_NODE,
        INTRODUCE_EDGE,
        FORGET_NODE,
        MERGE,
    };

    Bag(Bag::BagType type, Bag* parent, Bag** ref, const std::vector<Node*>& nodes);
    
    Bag(Bag::BagType type, Node* introduced_node);
    Bag(Bag::BagType type, std::pair<Node*, Node*> introduced_edge, int edge_weight);
    Bag(Bag::BagType type);

    std::vector<Node*> nodes;
    BagType type;
    Bag* parent;
    Bag* left;
    Bag* right;

    Node* introduced_node;
    Node* forgotten_node;
    std::pair<Node*, Node*> introduced_edge;
    long long int edge_weight;

    std::vector<Bag*> Generate(const std::vector<Bag::BagType>& types, int probability);

    std::string Label() const;
    std::string ToString() const;
    void Print() const;
    void print();

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
    // Arg 1: number of bags whose children have generated type -
    // total number of bags is sum of the number and all "introduce subtrees"
    // increased by 5 (root with its following forget bags), 
    // Arg 2: probability per node it's a terminal
    void Generate(int, int);
    // Bags should have only type and terminal set.
    Tree(std::vector<Bag*> &bags);
    // Arg: probability yes [0,100]
    void IntroduceEdges(int);
    void DotTransitionGraph(std::string file_path);
    int GetGraphSize();
    int GetTreeSize();
    int GetTreeWidth();
    void AddNodeToAllBags(Bag* b, Node* n);
};

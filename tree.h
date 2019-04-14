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
        LEAF,
        INTRODUCE_NODE,
        INTRODUCE_EDGE,
        FORGET_NODE,
        MERGE,
    };

    Bag(Bag::BagType type, Bag* parent, Bag** ref, const std::vector<Node*>& nodes);

    std::vector<Node*> nodes;
    BagType type;
    Bag* parent;
    Bag* left;
    Bag* right;

    Node* introduced_node;
    Node* forgotten_node;
    std::pair<Node*, Node*> introduced_edge;

    std::vector<Bag*> Generate(const std::vector<Bag::BagType>& types, int probability);

    std::string Label() const;
    std::string ToString() const;
    void Print() const;

    int rank;
    int id;

};

class Tree {
public:
    Tree() {}
    Bag* root;
    int max_width = 5;

    void Generate(int, int);
    // Arg: probability yes [0,100]
    void IntroduceEdges(int);
    void DotTransitionGraph(std::string file_path);
};

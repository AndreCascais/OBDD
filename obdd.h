#include <cstddef>
#include <iostream>
#include <string.h>
#include <map>
#include <functional>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct Node {
    char var;
    int label;
    int visited = 0;
    Node* left;
    Node* right;
};

void print_node(Node* node);
bool is_leaf(Node* node);
int And(int n1, int n2);
int Or(int n1, int n2);

class OBDD {
public:
    OBDD(int n, char* v);
    ~OBDD();
    void destroy_OBDD();
    void make_OBDD();
    void iterate_OBDD();
    void reduce();
    void print_obdd();
    void apply(std::function<int (int, int)> op, OBDD* f, OBDD* g);

    int n_vars;
    char* vars;

private:
    void destroy_OBDD(Node* node);
    void make_OBDD(int depth, Node* node);
    void copy_OBDD(OBDD* obdd);
    void delete_node(Node* node);
    int get_label(Node* left, Node* right);
    void label_OBDD(Node* node);
    void reduce(Node* node);
    void print_obdd(Node* n);
    Node* apply(std::function<int (int, int)> op, Node* f, Node* g);

    Node* root;
    std::map<char, int> var_map; // Maps each var to a int according to the obbd order
    std::map<int, Node*> node_map; // For each label associeates a single node
};

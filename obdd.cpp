#include <cstddef>
#include <iostream>
#include <string.h>
#include <map>

#include <stdio.h>
#include <stdlib.h>

// global input file
FILE * fp;

using namespace std;

struct Node {
    char var;
    int label;
    Node* left;
    Node* right;
};

void print_node(Node* node) {
    printf("My var is %c, my label is %d\n", node->var, node->label);
    if (node->left == NULL) {
        printf("Fim\n");
        return;
    }
    else if (node->right == NULL)
        printf("Pois\n");
}

class Btree {
public:
    Btree(int n, char* v);
    ~Btree();
    void destroy_tree();
    void make_tree();
    void iterate_tree();
    void reduce();

private:
    void destroy_tree(Node* node);
    void make_tree(int depth, Node* node);
    void delete_node(Node* node);
    int get_label(Node* left, Node* right);
    void label_tree(Node* node);
    void reduce(Node* node);

    Node* root;
    int n_vars;
    char* vars;
    int next_label;
    std::map<std::pair<int, int>, int> label_map; // Map to find a label given left and right ones
    std::map<int, Node*> node_map; // For each label associeates a single node
};

Btree::Btree(int n, char* v) {
    root = NULL;
    n_vars = n;
    vars = (char*) malloc (n_vars * sizeof(char));
    strcpy(vars, v);
    next_label = 2;

    Node* node_0 = new Node;
    Node* node_1 = new Node;

    node_0->left = NULL;
    node_0->right = NULL;
    node_0->label = 0;

    node_1->left = NULL;
    node_1->right = NULL;
    node_1->label = 1;

    node_map[0] = node_0;
    node_map[1] = node_1;
}

Btree::~Btree() {
    destroy_tree();
}

void Btree::destroy_tree() {
    destroy_tree(root);
}

void Btree::destroy_tree(Node* node) {

    if(node != NULL) {
        destroy_tree(node->left);
        destroy_tree(node->right);
        delete node;
    }
}

void Btree::make_tree() {

    make_tree(1, NULL);
}

void Btree::make_tree(int depth, Node* node) {

    if (depth == 1) {
        root = new Node;
        node = root;
    }

    node->left = new Node;
    node->right = new Node;
    node->var = vars[depth - 1];

    if (depth == n_vars) {

        node->left->left = NULL;
        node->left->right = NULL;
        fscanf(fp, "%d", &node->left->label);
        //node->left->var = '0' + node->left->label; // Just for debug

        node->right->left = NULL;
        node->right->right = NULL;
        fscanf(fp, "%d", &node->right->label);
        //node->right->var = '0' + node->right->label; // Just for debug
        return;
    }

    make_tree(depth + 1, node->left);
    make_tree(depth + 1, node->right);

}

void Btree::iterate_tree() {
    Node* n = root;

    printf("l - left\n");
    printf("r - right\n");
    printf("root - root\n");

    char s[10];
    while (1) {

        printf("My var is %c, my label is %d\n", n->var, n->label);
        scanf("%s", s);

        if (strcmp(s, "l") == 0) {
            if (n->left == NULL)
                printf("Not going to NULL Node\n");
            else
                n = n->left;
        }
        else if (strcmp(s, "r") == 0) {
            if (n->right == NULL)
                printf("Not going to NULL Node\n");
            else
                n = n->right;
        }
        else if (strcmp(s, "root") == 0) {
            n = root;
        }
        else
            printf("Unknown cmd\n");
    }
}

void Btree::label_tree(Node *node) {

    if (node->left != NULL) { // Still have stuff to label

        if (node->left->left != NULL) { // Next is not terminal
            label_tree(node->left);
            label_tree(node->right);
        }
        node->label = get_label(node->left, node->right);
    }
}

int Btree::get_label(Node *left, Node*right) {

    std::pair<int, int> label_pair = std::make_pair(left->label, right->label);
    if (left->label == right->label)
        return left->label;
    if (label_map.count(label_pair) == 0) {
        label_map[label_pair] = next_label;
        return next_label++;
    }
    else
        return label_map[label_pair];

}

void Btree::reduce() {
    label_tree(root);
    reduce(root);
}

void Btree::reduce(Node *node) { // Assumes labeled obdd

    if (node->left == NULL)// At a leaf
        return;

    while(node->left->label == node->right->label) {
        destroy_tree(node->right);
        delete_node(node);
        if (node->left == NULL) // Reached a leaf
            return;
    }

    if (node_map.count(node->label) == 0) // Label isnt maped to a node yet
        node_map[node->label] = node;

    if (node_map.count(node->left->label) != 0) { // Left label is already maped to a node
        int label = node->left->label;
        destroy_tree(node->left);
        node->left = node_map[label];
    }

    else
        reduce(node->left);

    if (node_map.count(node->right->label) != 0) { // Right label is already maped to a node
        int label = node->right->label;
        destroy_tree(node->right);
        node->right = node_map[label];
    }

    else
        reduce(node->right);
}

void Btree::delete_node(Node* node) { // In this context deleting a node is copying the information from its left node

    Node* node_aux = node->left;

    node->var = node_aux->var;
    node->right = node_aux->right;
    node->left = node_aux->left;
    node->label = node_aux->label;

    free(node_aux);
}

int main(int argc, char** argv) {

    fp = fopen(argv[1], "r+");
    int n_vars;
    fscanf(fp, "%d", &n_vars);
    char* vars = (char*) malloc (n_vars * sizeof(char));
    fscanf(fp, "%s", vars);

    Btree* obdd = new Btree(n_vars, vars);
    obdd->make_tree();
    obdd->reduce();
    obdd->iterate_tree();

    return 0;
}

#include <cstddef>
#include <iostream>
#include <string.h>
#include <map>

#include <stdio.h>
#include <stdlib.h>

// global input file
FILE * fp;

using namespace std;

struct node {
    char var;
    int label;
    node* left;
    node* right;
    node* father;
};

class btree {
public:
    btree(int n, char* v);
    ~btree();
    void destroy_tree();
    void make_tree(int depth, node* leaf);
    void iterate_tree();
    void label_tree(node* leaf);
    int get_label(node* left, node* right);

private:
    void destroy_tree(node* leaf);

    node* root;
    int n_vars;
    char* vars;
    int next_label;
    std::map<std::pair<int, int>, int> label_map;
};

btree::btree(int n, char* v) {
    root = NULL;
    n_vars = n;
    vars = (char*) malloc (n_vars * sizeof(char));
    strcpy(vars, v);
    next_label = 2;
}

btree::~btree() {
    destroy_tree();
}

void btree::destroy_tree() {
    destroy_tree(root);
}

void btree::destroy_tree(node* leaf) {

    if(leaf != NULL) {
        destroy_tree(leaf->left);
        destroy_tree(leaf->right);
        delete leaf;
    }
}

void btree::make_tree(int depth, node* leaf) {

    if (depth == 1) {
        root = new node;
        root->father = NULL;
        leaf = root;
    }

    leaf->left = new node;
    leaf->right = new node;
    leaf->left->father = leaf;
    leaf->right->father = leaf;
    leaf->var = vars[depth - 1];

    if (depth == n_vars) {
        
        leaf->left->left = NULL;
        leaf->left->right = NULL;
        fscanf(fp, "%d", &leaf->left->label);
        leaf->left->var = '0' + leaf->left->label; // Just for debug
        
        leaf->right->left = NULL;
        leaf->right->right = NULL;
        fscanf(fp, "%d", &leaf->right->label);
        leaf->right->var = '0' + leaf->right->label; // Just for debug
        return;
    }

    make_tree(depth + 1, leaf->left);
    make_tree(depth + 1, leaf->right);

}

void btree::iterate_tree() {
    node* n = root;
    
    printf("f - father\n");
    printf("l - left\n");
    printf("r - right\n");

    char s[10];
    while (1) {

        printf("My var is %c, my label is %d\n", n->var, n->label);
        scanf("%s", s);
        
        if (strcmp(s, "l") == 0) {
            if (n->left == NULL)
                printf("Not going to NULL node\n");
            else
                n = n->left;
        }
        else if (strcmp(s, "r") == 0) {
            if (n->right == NULL)
                printf("Not going to NULL node\n");
            else
                n = n->right;
        }
        else if (strcmp(s, "f") == 0) {
            if (n->father == NULL)
                printf("Not going to NULL node\n");
            else
                n = n->father;
        }
        else
            printf("Unknown cmd\n");
    }
}

void btree::label_tree(node *leaf) {

    if (leaf == NULL)
        leaf = root;
    if (leaf->left != NULL) { // Still have stuff to label

        if (leaf->left->left != NULL) { // Next is not terminal
            label_tree(leaf->left);
            label_tree(leaf->right);
        }
        leaf->label = get_label(leaf->left, leaf->right);
    }
}

int btree::get_label(node *left, node*right) {

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
int main(int argc, char** argv) {

    fp = fopen(argv[1], "r+");
    int n_vars;
    fscanf(fp, "%d", &n_vars);
    char* vars = (char*) malloc (n_vars * sizeof(char));
    fscanf(fp, "%s", vars);

    btree* obdd = new btree(n_vars, vars);
    obdd->make_tree(1, NULL);
    obdd->label_tree(NULL);
    obdd->iterate_tree();

    return 0;
}

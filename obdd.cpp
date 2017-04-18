#include <cstddef>
#include <iostream>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct node {
    char label;
    node *left;
    node *right;
};

	
class btree {
public:
    btree(int n_vars);
    ~btree();
    void destroy_tree();

private:
    void destroy_tree(node *leaf);
    void make_tree();

    node *root;
    int n_vars;
    char *vars;
};

btree::btree(int n) {
    root = NULL;
    n_vars = n;
    vars = (char*) malloc (n_vars * sizeof(char));
}

btree::~btree() {
    destroy_tree();
}

void btree::destroy_tree() {
    destroy_tree(root);
}

void btree::destroy_tree(node *leaf) {

    if(leaf != NULL) {
        destroy_tree(leaf->left);
        destroy_tree(leaf->right);
        delete leaf;
    }
}

void btree::make_tree() {
    
}

int main(int argc, char** argv) {
    int n_vars;
    scanf("%d", &n_vars);
    char* vars = (char*) malloc (n_vars * sizeof(char));
    scanf("%s", vars);


    printf("%d vars, %s\n", n_vars, vars);
    return 0;
}

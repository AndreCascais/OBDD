#include "obdd.h"

using namespace std;

FILE * fp; // global input file
FILE * of; // global output file
std::map<string, OBDD*> obdd_map; // Maps an id into a OBDD (unique)
std::map<string, std::function<int (int, int)>> function_map; // Map strings to functions
std::map<std::pair<int, int>, int> label_map; // Map to find a label given left and right ones
int next_label = 2;


OBDD::OBDD(int n, char* v) {
  root = NULL;
  n_vars = n;
  vars = (char*) malloc (n_vars * sizeof(char));
  strcpy(vars, v);

  int i;
  for(i = 0; i < n_vars; i++) {
    var_map[vars[i]] = i;
  }

  var_map['\0'] = ++i;

  Node* node_0 = new Node;
  Node* node_1 = new Node;

  node_0->left = NULL;
  node_0->right = NULL;
  node_0->label = 0;
  node_0->var = '\0';
  node_0->visited = 1;

  node_1->left = NULL;
  node_1->right = NULL;
  node_1->label = 1;
  node_1->var = '\0';
  node_1->visited = 1;

  node_map[0] = node_0;
  node_map[1] = node_1;
}

OBDD::~OBDD() {
  destroy_OBDD();
}

void OBDD::destroy_OBDD() {
  destroy_OBDD(root);
  delete node_map[0];
  delete node_map[1];
}

void OBDD::destroy_OBDD(Node* node) {

  if(node != NULL && node->visited == 0) {
    destroy_OBDD(node->left);
    destroy_OBDD(node->right);
    delete node;
  }
}

void OBDD::make_OBDD() {

  make_OBDD(1, NULL);
}

void OBDD::make_OBDD(int depth, Node* node) {

  if (depth == 1) {
    root = new Node;
    node = root;
  }

  node->left = new Node;
  node->right = new Node;
  node->var = vars[depth - 1];
  node->visited = 0;

  if (depth == n_vars) {

    node->left->left = NULL;
    node->left->right = NULL;
    fscanf(fp, "%d", &node->left->label);

    node->right->left = NULL;
    node->right->right = NULL;
    fscanf(fp, "%d", &node->right->label);

    return;
  }

  make_OBDD(depth + 1, node->left);
  make_OBDD(depth + 1, node->right);

}

void OBDD::iterate_OBDD() {
  Node* n = root;

  printf("l - left\n");
  printf("r - right\n");
  printf("root - root\n");
  printf("quit - quit\n");

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
    else if (strcmp(s, "quit") == 0) {
      return;
    }
    else
      printf("Unknown cmd\n");
  }
}

void OBDD::label_OBDD(Node *node) {

  if (node->left != NULL) { // Still have stuff to label

    if (node->left->left != NULL) { // Next is not terminal
      label_OBDD(node->left);
      label_OBDD(node->right);
    }
    node->label = get_label(node->left, node->right);
    node->visited = 0; // Useful reset tool for reduce
  }
}

int OBDD::get_label(Node *left, Node*right) {

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

void OBDD::reduce() {
  label_OBDD(root);
  Node* node_0 = node_map[0];
  Node* node_1 = node_map[1];
  node_map.clear();
  node_map[0] = node_0;
  node_map[1] = node_1;
  clear_visited(root);
  reduce(root);
}

void OBDD::reduce(Node *node) { // Assumes labeled obdd
  node->visited = 1;

  if (node->left == NULL)// At a leaf
    return;

  while (node->left->label == node->right->label) {
    destroy_OBDD(node->right);
    delete_node(node);
    if (node->left == NULL) // Reached a leaf
      return;
  }

  if (node_map.count(node->label) == 0) // Label isnt maped to a node yet
    node_map[node->label] = node;

  if (node_map.count(node->left->label) != 0) { // Left label is already maped to a node
    int label = node->left->label;
    destroy_OBDD(node->left);
    node->left = node_map[label];
  }

  else
    reduce(node->left);

  if (node_map.count(node->right->label) != 0) { // Right label is already maped to a node
    int label = node->right->label;
    destroy_OBDD(node->right);
    node->right = node_map[label];
  }

  else
    reduce(node->right);
}

void OBDD::delete_node(Node* node) { // In this context deleting a node is copying the information from its left node

  Node* node_aux = node->left;

  node->var = node_aux->var;
  node->right = node_aux->right;
  node->left = node_aux->left;
  node->label = node_aux->label;

  free(node_aux);
}
void OBDD::print_obdd() {
  fprintf(of, "digraph {\n");
  print_obdd(root);
  fprintf(of, "}");
  fclose(of);
  return;
}

void OBDD::print_obdd(Node *n){
  //print to outputfile an edge from n to left and from n to right in .dot format
  n -> visited++;
  if(n -> left == NULL && n -> right == NULL){
    fprintf(of, "{%d[shape=\"square\"]}\n", n->label);
    return;
  }
  //printf("%c %c %c %d %d %d\n", n->var, n->left->var, n->right->var, n->label, n->left->label, n->right->label);
  if(n -> visited < 2){
    if(n -> left -> var != '\0'){
      
      fprintf(of, "{%d[label=\"%c\"]} -> {%d[label=\"%c\"]}[style=\"dashed\"]\n", n -> label, n -> var, n -> left -> label, n -> left -> var);
      print_obdd(n -> left);
    }
  
    else{
      fprintf(of, "{%d[label=\"%c\"]} -> {%d [shape=\"square\"]} [style=\"dashed\"]\n", n -> label, n -> var, n -> left -> label);
    }
  }
  if(n->visited < 2){
    if(n -> right -> var != '\0'){
    
      fprintf(of, "{%d[label=\"%c\"]} -> {%d[label=\"%c\"]}\n", n -> label, n -> var, n -> right -> label, n -> right -> var);
    
      print_obdd(n -> right);
    }
  
    else{
      fprintf(of, "{%d[label=\"%c\"]} -> {%d [shape=\"square\"]}\n", n -> label, n -> var, n -> right -> label);
    }
  }
  return;
}

void OBDD::launch_viewer(char *filename){
  pid_t pid;
  int status;
  int child_pid = 0;
  if(!(pid = fork())){
    child_pid = getpid();
    execlp("dot", "dot", "-Tpng", "-O", filename, NULL);
  }
  else{
    // strcat(filename, ".png"); // concat the image extension to filename
    //execlp("showimage", "showimage", filename, NULL); // uncomment if wanna launch the viewer from here 
    waitpid(child_pid, &status, WCONTINUED);
  }
  return;
}

void OBDD::make_partial_obdd(OBDD* o, int value, char* var){
  root = make_partial_obdd(o->root, value, var[0]);
}

Node* OBDD::make_partial_obdd(Node *n, int v, char c){
  Node *node = new Node;
  if(n == NULL) return NULL;
  if(is_leaf(n)){
    node->left = NULL;
    node->label = n->label;
    node->var = n->var;
    node->right = NULL;
  }

  else if(n->var == c){
    if(v){
      node -> var = n-> right-> var;
      node -> label = n-> right -> label;
      node->left = make_partial_obdd(n->right->left, v, c);
      node->right = make_partial_obdd(n->right->right, v, c);
    }
    else{
      node -> var = n-> left-> var;
      node -> label = n-> left-> label;
      node->left = make_partial_obdd(n->left->left, v, c);
      node->right = make_partial_obdd(n->left->right, v, c);
    }
  }

  else{
    node -> var = n -> var;
    node -> label = n -> label;
    node->left = make_partial_obdd(n->left, v, c);
    node->right = make_partial_obdd(n->right, v, c);
  }
  return node;
}

void OBDD::apply(std::function<int (int, int)> op, OBDD* f, OBDD* g) {

  root = apply(op, f->root, g->root);
}

Node* OBDD::apply(std::function<int (int, int)> op, Node* f, Node* g) {

  Node* node = new Node;

  if (is_leaf(f) && is_leaf(g)) {
        
    node->left = NULL;
    node->right = NULL;

    int res = op(f->label, g->label);

    node->label = res;
    node->var = '\0';
  }

  else if (f->var == g->var) {

    node->label = f->label;
    node->var = f->var;
        
    node->left = apply(op, f->left, g->left);
    node->right = apply(op, f->right, g->right);
  }

  else {
        
    Node* high;
    Node* low;

    if (var_map[f->var] > var_map[g->var]) {
      high = f;
      low = g;
    }
        
    else {
      high = g;
      low = f;
    }

    node->var = low->var;
    node->label = low->label;
    node->left = apply(op, low->left, high);
    node->right = apply(op, low->right, high);
        
  }
   
  return node;
}

void OBDD::clear_visited() {
  clear_visited(root);
}

void OBDD::clear_visited(Node* n) {
  if (is_leaf(n))
    return;
  else {
    n->visited = 0;
    clear_visited(n->left);
    clear_visited(n->right);
  }
}

void print_node(Node* node) {

  printf("My var is %c, my label is %d\n", node->var, node->label);
}

bool is_leaf(Node* node) {

  return node->left == NULL;
}

int And(int n1, int n2) {
  return n1 && n2;
}

int Or(int n1, int n2) {
  return n1 || n2;
}

char *remove_variable(char *vars, char *v){
  char *c = (char *) malloc(sizeof(vars));
  strcpy(c, vars);
  bool found = false;
  
  unsigned int i;
  for(i = 0; i < sizeof(vars); i++) {
    if(c[i] == v[0]){
      found = true;
    }
    else if(found){
      c[i-1] = c[i];
    }
  }
  return c;
}

void run() {

  char cmd[100];
  char const* help_msg = "help                       - shows this message\n"
    "list                       - lists available obdds\n"
    "iterate OBDD               - iterates obbd named OBDD\n"
    "new OBDD FILE              - creates a new obbd named OBDD from file FILE\n"
    "show OBDD FILE             - creates .dot file for OBDD in file FILE\n"
    "reduce OBDD                - reduces obdd OBDD\n"
    "apply OP OBDD1 OBDD2 OBDD3 - apply(op, OBDD1, OBDD2) into OBDD3\n"
    "exists OP VAR OBDD1 OBDD2  - exists(op, OBDD1[0/VAR], OBDD1[1/VAR]) into OBDD2\n"
    "forall OP VAR OBDD1 OBDD2  - forall(op, OBDD1[0/VAR], OBDD1[1/VAR]) into OBDD2\n"
    ;
    
  printf("%s", help_msg);

  while(1) {
 
    fgets (cmd, 100, stdin);

    char* token = strtok(cmd, " \n");

    if (token == NULL)
      continue;

    if (strcmp(token, "help") == 0) {
      printf("%s", help_msg);
    }

    else if (strcmp(token, "list") == 0) {
      if (obdd_map.empty())
	printf("No obbds yet\n");
            
      else {
	for(map<string, OBDD*>::const_iterator it = obdd_map.begin(); it != obdd_map.end(); ++it)
	  {
	    printf("%s - vars <- [%s]\n", it->first.c_str(), it->second->vars);
	  }
      }
    }
        
    else if (strcmp(token, "new") == 0) {
            
      char file[100];
            
      token = strtok(NULL, " \n");
      string id(token);
      char *name = token;
      token = strtok(NULL, " \n");
      strcpy(file, token);

      if (obdd_map.count(id) == 0) {
	fp = fopen(file, "r+");
	if(fp == NULL){
	  printf("File %s doesn't exist\n", token);
	  return;
	}
	int n_vars;
	fscanf(fp, "%d", &n_vars);
	char* vars = (char*) malloc (n_vars * sizeof(char));
	fscanf(fp, "%s", vars);
            
	OBDD* obdd = new OBDD(n_vars, vars);
	obdd->make_OBDD();
	obdd_map[id] = obdd;
	fclose(fp);
	printf("%s created with success\n", name);
      }

      else
	printf("Id already in use\n");
    }
        
    else if (strcmp(token, "iterate") == 0) {
      OBDD* obdd;

      token = strtok(NULL, " \n");
      string id(token);

      obdd = obdd_map[id];
            
      obdd->iterate_OBDD();
    }

    else if (strcmp(token, "show") == 0) {
      OBDD* obdd;
            
      token = strtok(NULL, " \n");
      string id(token);
      token = strtok(NULL, " \n");
      of = fopen(token, "w+");
                
      obdd = obdd_map[id];
      obdd->reduce();
      obdd->clear_visited();
      obdd->print_obdd();
      // printf("entrei no launcher\n");
      obdd->launch_viewer(token);
      //printf("sai do launcher\n");
    }

    else if (strcmp(token, "reduce") == 0) {
      OBDD* obdd;

      token = strtok(NULL, " \n");
      string id(token);
      obdd = obdd_map[id];
      obdd->reduce();
      printf("%s reduced with success\n", token);
    }
        
    else if (strcmp(token, "apply") == 0) {

      token = strtok(NULL, " \n");
      string op(token);
      token = strtok(NULL, " \n");
      string id1(token);
      token = strtok(NULL, " \n");
      string id2(token);
      token = strtok(NULL, " \n");
      string id3(token);
            
      OBDD* obdd1 = obdd_map[id1];
      OBDD* obdd2 = obdd_map[id2];
            
      OBDD* obdd3 = new OBDD(obdd1->n_vars, obdd1->vars);
      obdd3->apply(function_map[op], obdd1, obdd2);
      obdd_map[id3] = obdd3;
    }

    else if(!strcmp(token, "exists")){
      token = strtok(NULL, " \n");
      char *var = token;
      token = strtok(NULL, " \n");
      string id1(token);
      /* char *name = token;
	string id2(strcat(token,"[0]"));
	string id3(strcat(name,"[1]"));*/
      token = strtok(NULL, " \n");
      string id4(token);

      OBDD *o1 = obdd_map[id1];
      char *new_vars = remove_variable(o1->vars, var);
      OBDD *o2 = new OBDD(o1->n_vars -1, new_vars);
      o2 -> make_partial_obdd(o1, 0, var);
      // obdd_map[id2] = o2;
      OBDD *o3 = new OBDD(o1->n_vars -1, new_vars);
      o3 -> make_partial_obdd(o1, 1, var);
      // obdd_map[id3] = o3;  
      OBDD *o4 = new OBDD(o1->n_vars -1, new_vars);
      o4 -> apply(Or, o2, o3);
      //printf("sai\n");
      obdd_map[id4] = o4;
    }
	
    else if(!strcmp(token, "forall")){
      token = strtok(NULL, " \n");
      char *var = token;
      token = strtok(NULL, " \n");
      string id1(token);
      // string id2(strcat(token,"[0]"));
      // string id3(strcat(token,"[1]"));
      token = strtok(NULL, " \n");
      string id4(token);

      OBDD *o1 = obdd_map[id1];
      char *new_vars = remove_variable(o1->vars, var);
      OBDD *o2 = new OBDD(o1->n_vars -1, new_vars);
      o2 -> make_partial_obdd(o1, 0, var);
      // obdd_map[id2] = o2;
      OBDD *o3 = new OBDD(o1->n_vars -1, new_vars);
      o3 -> make_partial_obdd(o1, 1, var);
      // obdd_map[id3] = o3;
      OBDD *o4 = new OBDD(o1->n_vars -1, new_vars);
      o4 -> apply(And, o2, o3);
      obdd_map[id4] = o4;
    }

    else
      printf("Unknown cmd\n");

  }
}


int main(int argc, char** argv) {

  function_map["and"] = And;
  function_map["or"] = Or;
  run();

  return 0;
}

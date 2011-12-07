
#define MAX_LEVEL 5

struct node {
  int key;
  int topLevel;
  struct node* next[MAX_LEVEL];
  int prefix[MAX_LEVEL];
};

class SkipList {
 public:
  SkipList();
  ~SkipList();

  int lookup(int key);
  int insert(int key);

  void print_skiplist();
  void pretty_print_skiplist();

 private:
  int findNode(int key, node* preds[], node* succs[]);
  node* head;
  node* tail;
};

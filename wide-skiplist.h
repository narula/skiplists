
#define MAX_LEVEL 20

struct node {
  int key;
  int topLevel;
  struct node* next[MAX_LEVEL];
  struct node* extra[MAX_LEVEL];
  int prefix[MAX_LEVEL];
  int extra_prefix[MAX_LEVEL];
};

class WideSkipList {
 public:
  WideSkipList(int prob, int sz);
  ~WideSkipList();

  int lookup(int key);
  int insert(int key);

  void print_skiplist();
  void pretty_print_skiplist();

 private:
  int findNode(int key, node* preds[], node* succs[]);
  node* head;
  node* tail;
  int probability;
  int max_level;
};

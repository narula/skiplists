
#define MAX_LEVEL 20

struct next {
  int prefix;
  struct node* nxt;
};

struct node {
  int key;
  int topLevel;
  struct next nexts[MAX_LEVEL];
  next* nn(int level) {
	return &(nexts[topLevel - 1 - level]);
  }
};

class SkipList {
 public:
  SkipList(int prob, int maxl);
  ~SkipList();

  int lookup(int key);
  int insert(int key);

  void print_skiplist();
  void pretty_print_skiplist();

  static SkipList* init_list(int sz, int high, int probability);

 private:
  int findNode(int key, node* preds[], node* succs[]);
  node* head;
  node* tail;
  int probability;
  int max_level;
};

// Proper read/write non-concurrent skiplist, can change the base for
// levels.
// ./skiplist size base

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
	//	return &(nexts[topLevel - 1 - level]);
	return &(nexts[level]);
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
  void enable_counts() { count = 1; };
  void disable_counts() { count = 0; } ;
  void inc() { if (count == 1) pointer_follows++; };
  int get_ptr_count() { return pointer_follows; };

  static SkipList* init_list(int sz, int high, int probability);

 private:
  int findNode(int key, node* preds[], node* succs[]);
  node* head;
  node* tail;
  int probability;
  int max_level;
  int pointer_follows;
  int count;
};

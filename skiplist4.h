
#define MAX_LEVEL 4

struct next4 {
  int prefix;
  struct node4* nxt;
};

struct node4 {
  int key;
  int topLevel;
  struct next4 nexts[4][MAX_LEVEL];
  next4* nn(int level, int ptr) {
	return &(nexts[ptr][level]);
	//	return &(nexts[ptr][topLevel - 1 - level]);
  }
};

class SkipList4 {
 public:
  SkipList4();
  SkipList4(int prob, int maxl);
  ~SkipList4();

  int lookup(int key);
  int insert(int key);

  void print_skiplist();
  void pretty_print_skiplist();
  node4* head;
  node4* tail;
  int probability;
  int max_level;


  static SkipList4* init_list(int sz);

 private:
  int findNode(int key, node4* preds[], node4* succs[]);
};

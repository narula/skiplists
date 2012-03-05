// Hacked up, read-only fast skiplist by 2 with no inserts.

#define MAX_LEVEL 27

struct next2 {
  int prefix;
  struct node2* nxt;
};

struct node2 {
  int key;
  int topLevel;

  struct next2 nexts[MAX_LEVEL];
};

class SkipList2 {
 public:
  SkipList2();
  SkipList2(int prob, int maxl);
  ~SkipList2();

  int lookup(int key);
  void enable_counts() { count = 1; };
  void disable_counts() { count = 0; } ;
  void inc() { if (count == 1) pointer_follows++; };
  int get_ptr_count() { return pointer_follows; };

  void printlist();
  void pretty_print_skiplist();
  node2* head;
  node2* tail;
  int probability;
  int max_level;


  static SkipList2* init_list(int sz, int maxl);

 private:
  int findNode(int key, node2* preds[], node2* succs[]);
  int pointer_follows;
  int count;
};

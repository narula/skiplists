// Hacked up, read-only fast skiplist by FANOUT with no inserts.

#define MAX_LEVEL 13
#define FANOUT 4

struct next {
  int prefix;
  struct node* nxt;
};

struct node {
  int key;
  int topLevel;

  // FANOUT total pointers.  nexts[0] is the traditional one in
  // a skiplist.  the other ones point ahead.
  struct next nexts[MAX_LEVEL][FANOUT];
};

class SkipList {
 public:
  SkipList();
  SkipList(int maxl);
  ~SkipList();

  int lookup(int key);
  int insert(int key);
  void enable_counts() { count = 1; };
  void disable_counts() { count = 0; } ;
  void inc() { if (count == 1) pointer_follows++; };
  int get_ptr_count() { return pointer_follows; };

  void detailed_print();
  void pretty_print();
  node* head;
  node* tail;
  int max_level;

  static SkipList* init_list(int sz, int high, int maxl);

 private:
  int fastFindNode(int key);
  int findNode(int key, node* preds[][FANOUT], node* succs[][FANOUT]);
  int pointer_follows;
  int count;
};

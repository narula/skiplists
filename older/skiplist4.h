// Hacked up, read-only fast skiplist by 4 with no inserts.

#define MAX_LEVEL 13

struct next4 {
  int prefix;
  struct node4* nxt;
};

struct node4 {
  int key;
  int topLevel;

  // 4 total pointers.  nexts[3] is the traditional one in a skiplist.
  // the other ones point inside.
  struct next4 nexts[MAX_LEVEL][4];
};

class SkipList4 {
 public:
  SkipList4();
  SkipList4(int prob, int maxl);
  ~SkipList4();

  int lookup(int key);
  void enable_counts() { count = 1; };
  void disable_counts() { count = 0; } ;
  void inc() { if (count == 1) pointer_follows++; };
  int get_ptr_count() { return pointer_follows; };

  void printlist();
  void pretty_print_skiplist();
  node4* head;
  node4* tail;
  int probability;
  int max_level;


  static SkipList4* init_list(int sz, int maxl);

 private:
  int findNode(int key);
  int pointer_follows;
  int count;
};

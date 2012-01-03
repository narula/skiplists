// Hacked up, read-only, skinny fast skiplist by 4 with no inserts.

#define MAX_LEVEL 20

struct next4 {
  int prefix;
  struct node4* nxt;
};

struct node4 {
  int key;
  int topLevel;
  struct node4* down;

  // 4 total pointers.  nexts[3] is the traditional one in a skiplist.
  // the other ones point inside.
  struct next4 nexts[4];
};

class SkipList4Skinny {
 public:
  SkipList4Skinny();
  SkipList4Skinny(int prob, int maxl);
  ~SkipList4Skinny();

  int lookup(int key);

  void printlist();
  void pretty_print_skiplist();
  node4* head;
  node4* tail;
  int probability;
  int max_level;


  static SkipList4Skinny* init_list(int sz, int maxl);

 private:
  int findNode(int key);
};

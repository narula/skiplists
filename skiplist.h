
#define MAX_LEVEL 5

struct nextinfo {
  struct node* next;
  int prefix_hint;
};

struct node {
  int key;
  struct nextinfo nexts[MAX_LEVEL];
};

class SkipList {
 public:
  SkipList();
  ~SkipList();

  int lookup(int key);
  int insert(int key);

  void print_skiplist();

 private:
  int findNode(int key, node* preds[], node* succs[]);
  node* head;
  node* tail;
};

#if SKIPLIST
//
// Herlihy &c, "A provably correct scalable concurrent skip list"
//

struct timedvalue {
 kvtimestamp_t ts;
 int vallen;
 char s[0];

 inline size_t size() const {
   return sizeof(timedvalue) + vallen + 1;
 }
};

static timedvalue *
make_timedvalue(kvtimestamp_t ts, const str &val, threadinfo *ti)
{
 timedvalue *tv = (timedvalue *) malloc(sizeof(timedvalue) + val.len + 1);
 assert(tv);
 tv->ts = ts;
 tv->vallen = val.len;
 memcpy(tv->s, val.s, val.len + 1);
 ti->mark_tree_alloc(sizeof(timedvalue) + val.len + 1);
 return tv;
}

// each node keeps not just a ptr to the next node at each
// layer, but also the first 8 bytes of the key of that next node.
// the point is to avoid the cache miss on that next node
// half the time.
// XXX this will probably make delete harder!
// XXX it may be that the hint breaks lookup/add concurrency,
//   since hint update in add() isn't atomic w/ p update.
struct nextinfo {
#define HINT sizeof(struct node *)
 char hint[HINT]; // first HINT bytes of target key
 struct node *p;
};

#define MaxHeight 20
struct node {
 char key[16];
 timedvalue *tv;
 char topLayer;
 volatile char marked; // bool
 volatile char fullyLinked; // bool
 char dummy;
 uint32_t lock;
 struct nextinfo nexts[0];
};

node *
newnode(const str &key, int topLayer, timedvalue *tv, threadinfo *ti)
{
 struct node *n = (struct node *) malloc(sizeof(struct node) +
                                     sizeof(struct nextinfo) * (topLayer+1));
 assert(n);
 ti->mark_tree_alloc(sizeof(struct node));
 assert(key.len < (int) sizeof(n->key));
 memcpy(n->key, key.s, key.len + 1);
 n->tv = tv;
 n->topLayer = topLayer;
 for(int i = 0; i <= topLayer; i++){
   n->nexts[i].p = 0;
   memset(n->nexts[i].hint, '\0', HINT);
 }
 n->marked = 0;
 n->fullyLinked = 0;
 n->lock = 0;
 return n;
}

void
check(struct node *root)
{
 struct node *n = root->nexts[0].p;
 struct node *prev = 0;
 while(strcmp(n->key, "~~~~~~~~") != 0){
   for(int i = 0; i <= n->topLayer; i++){
     struct node *p = n->nexts[i].p;
     assert(p);
     assert(memcmp(n->nexts[i].hint, p->key, HINT) == 0);
     if(strcmp(n->key, p->key) >= 0)
       printf("oops %s %s\n", n->key, p->key);
     assert(strcmp(n->key, p->key) < 0);
   }
   prev = n;
   n = n->nexts[0].p;
 }
}

// returns -1 or the highest layer in which the key was found.
// preds[] and succs[] contain the predecessor and successor
// at each layer.
// the "root" node's pred[0] and succ[0] are the very first
// and last nodes; they are fake.
int
findNode(struct node *root, const str &key,
        struct node *preds[], struct node *succs[])
{
 int lFound = -1;
 struct node *pred = root->nexts[0].p;
 for(int layer = MaxHeight-1; layer >= 0; --layer){
   struct node *curr = pred->nexts[layer].p;
   while(1){
     if(memcmp(key.s, pred->nexts[layer].hint, HINT) < 0 ||
        strcmp(key.s, curr->key) <= 0)
       break;
     pred = curr;
     curr = pred->nexts[layer].p;
   }
   if(lFound == -1 && strcmp(key.s, curr->key) == 0){
     lFound = layer;
   }
   preds[layer] = pred;
   succs[layer] = curr;
 }
 return lFound;
}

// per-thread 32-bit random numbers
// numerical recipes in c, page 284
unsigned int
trand(threadinfo *ti)
{
 ti->randstate = ti->randstate * 1664525 + 1013904223;
 return ti->randstate;
}

// return a random number 0..max-1
// 1/2 0
// 1/4 1
// 1/8 2
// &c
int
randomLevel(threadinfo *ti, int max)
{
 unsigned long long x = trand(ti);
 int layer = 0;
 while((x & 1) != 0){
   layer += 1;
   x >>= 1;
 }
 if(layer >= max)
   layer = max - 1;
 return layer;
}

// returns true if added, false if key already there.
int
add(struct node *root, const str &key,
   timedvalue *tv, threadinfo *ti)
{
 int topLayer = randomLevel(ti, MaxHeight);
 if(topLayer > root->topLayer)
   root->topLayer = topLayer;
 struct node *preds[MaxHeight], *succs[MaxHeight];
 while(1){
   int lFound = findNode(root, key, preds, succs);
   if(lFound != -1){
     struct node *nodeFound = succs[lFound];
     if(!nodeFound->marked){
       while(nodeFound->fullyLinked == 0)
         ;
       return 0;
     }
     continue;
   }
   int highestLocked = -1;
   struct node *pred, *succ, *prevPred = 0;
   int valid = 1;
   for(int layer = 0; valid && layer <= topLayer; layer++){
     pred = preds[layer];
     succ = succs[layer];
     if(pred != prevPred){
       while(xchg(&pred->lock, 1) == 1)
         ;
       highestLocked = layer;
       prevPred = pred;
     }
     valid = !pred->marked && !succ->marked &&
       pred->nexts[layer].p == succ;
   }
   if(!valid){
     for(int layer = 0; layer <= highestLocked; layer++)
       if(layer == 0 || preds[layer] != preds[layer-1])
         preds[layer]->lock = 0;
     continue;
   }
   struct node *newNode = newnode(key, topLayer, tv, ti);
   for(int layer = 0; layer <= topLayer; layer++){
     newNode->nexts[layer].p = succs[layer];
     memcpy(newNode->nexts[layer].hint, succs[layer]->key, HINT);
     preds[layer]->nexts[layer].p = newNode;
     memcpy(preds[layer]->nexts[layer].hint, newNode->key, HINT);
   }
   newNode->fullyLinked = true;
   for(int layer = 0; layer <= highestLocked; layer++)
     if(layer == 0 || preds[layer] != preds[layer-1])
       preds[layer]->lock = 0;
   return true;
 }
}

int
contains(struct node *root, const str &key, str &val)
{
 struct node *preds[MaxHeight], *succs[MaxHeight];
 int lFound = findNode(root, key, preds, succs);
 if(lFound != -1 &&
    succs[lFound]->fullyLinked &&
    succs[lFound]->marked == 0){
   timedvalue *tv = succs[lFound]->tv;
   val.assign(tv->s, tv->vallen);
   return 1;
 } else {
   printf("checking...\n");
   check(root);
   return 0;
 }
}

// insert or update
void
insert(node **rootp,
      const str &key,
      const str &val,
      kvtimestamp_t ts,
      threadinfo *ti)
{
 timedvalue *tv = make_timedvalue(ts, val, ti);
 int ret = add(*rootp, key, tv, ti);
 if(ret){
   // str xval;
   // assert(contains(*rootp, key, xval) == 1);
   // assert(strcmp(xval.s, val.s) == 0);
   logone(2, key, val, ts);
 } else {
   // XXX update existing node
   // XXX or at least free tv
   fprintf(stderr, "skiplist insert already there\n");
 }
}

bool
lookup(struct node *n, const str &key, str &val)
{
 if(contains(n, key, val)){
   return true;
 } else {
   return false;
 }
}

// in-order traversal with two tweaks:
//   don't bother with subtrees < startkey
//   quit after emitting numpairs
// XXX: nodes might be freed underfoot, RCU isn't
//   clever enough for scan().
int
scan(struct node *n, const str &startkey, int numpairs,
    void (*fn)(const str &k, const str &v))
{
 assert(0);
}

void
checkpoint_tree(struct node *n, struct ckstate *c)
{
 assert(0);
}

node *
tree_init(threadinfo *ti)
{
 struct node *n = newnode(str("xx"), MaxHeight-1, 0, ti);
 struct node *first = newnode(str(""), MaxHeight-1, 0, ti);
 struct node *last = newnode(str("~~~~~~~~"), MaxHeight-1, 0, ti);
 for(int layer = 0; layer < MaxHeight; layer++){
   n->nexts[layer].p = first;
   first->nexts[layer].p = last;
   memcpy(first->nexts[layer].hint, last->key, HINT);
   last->nexts[layer].p = 0;
 }
 return n;
}

#endif

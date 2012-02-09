#include "skiplist-extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>


SkipList::SkipList() {
  count = 0;
  pointer_follows = 0;
}

SkipList::SkipList(int maxl) {
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
  head = new node();
  tail = new node();
  head->key = -1;
  head->topLevel = max_level;
  tail->key = INT_MAX;
  tail->topLevel = max_level;
  for (int i = 0; i < MAX_LEVEL; i++) {
	for (int k = 0; k < FANOUT; k++) {
	  head->nexts[i][k].nxt = tail;
	  head->nexts[i][k].prefix = INT_MAX;
	}
  }
  count = 0;
  pointer_follows = 0;

}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::lookup(int key) {
  node *preds[max_level][FANOUT], *succs[max_level][FANOUT];
  //  if (findNode(key, preds, succs) >= 0) {
  if (fastFindNode(key) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

int randomLevel(int max) {
  unsigned long long x = rand();
  int toplayer = 1;
  switch (FANOUT) {
  case 4:
	while((x & 3) == 3){
	  toplayer += 1;
	  x >>= 2;
	}
	break;
  case 8:
	while((x & 7) == 7){
	  toplayer += 1;
	  x >>= 3;
	}
	break;
  case 16:
	while((x & 15) == 15){
	  toplayer += 1;
	  x >>= 4;
	}
	break;
  default:
	while((x & 1) != 0) {
	  toplayer += 1;
	  x >>= 1;
	}
	break;
  }
  if(toplayer > max)
	toplayer = max;
  return toplayer;
}

// Does not keep track of information for inserts.
int SkipList::fastFindNode(int key) {
  node* pred = head;
  int lFound = -1;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node* curr = pred->nexts[level][FANOUT-1].nxt;
	while (key > pred->nexts[level][FANOUT-1].prefix) {
	  pred = curr;
	  curr = pred->nexts[level][FANOUT-1].nxt;
	  inc();
	}
	int extra = 0;
	while (key > pred->nexts[level][extra].prefix) {
	  extra++;
	}
	if (lFound == -1 && key == pred->nexts[level][extra].prefix) {
	  return level;
	}
	curr = pred->nexts[level][extra].nxt;
	if (extra > 0) {
	  pred = pred->nexts[level][extra-1].nxt;
	  inc();
	}
  }
  return -1;
}

int SkipList::findNode(int key, node* preds[][FANOUT], node* succs[][FANOUT]) {
  node* pred = head;
  int lFound = -1;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node* curr = pred->nexts[level][FANOUT-1].nxt;
	next* bigFour = NULL;
	bool haveFour = false;
	while (key > pred->nexts[level][FANOUT-1].prefix) {
	  bigFour = pred->nexts[level];
	  pred = curr;
	  curr = pred->nexts[level][FANOUT-1].nxt;
	  inc();
	  haveFour = true;
	}
	int extra = 0;
	while (key > pred->nexts[level][extra].prefix) {
	  extra++;
	}
	curr = pred->nexts[level][extra].nxt;
	node* prev = pred;
	if (extra > 0) {
	  prev = pred->nexts[level][extra-1].nxt;
	}
	if (lFound == -1 && key == pred->nexts[level][extra].prefix) {
	  lFound = level;
	}
	succs[level][0] = curr;
	for (int i = 1; i < FANOUT; i++) {
	  succs[level][i] = prev->nexts[level][i].nxt;
	}
	// get the ones right before (extra)
	for (int i = 0; i < extra; i++) {
	  preds[level][i] = pred->nexts[level][extra-i-1].nxt;
	}
	if (haveFour == false) {
	  // did not make it past head
	  preds[level][extra] = pred;
	} else {
	  // either get last n where n = FANOUT-extra)  or whole thing backwards.
	  for (int i = extra; i < FANOUT; i++) {
		preds[level][i] = bigFour[FANOUT-i-1+extra].nxt;
	  }
	  // reset pred to be before
	  pred = preds[level][FANOUT-1];
	}
	assert(pred != NULL);
  }
  return lFound;
}

// Doesn't work.
int SkipList::insert(int key) {
  node *preds[max_level][FANOUT], *succs[max_level][FANOUT];
  for (int i = 0; i < max_level; i++) {
	for (int k = 0; k < FANOUT; k++) {
	  preds[i][k] = NULL;
	  succs[i][k] = NULL;
	}
  }
  int found = findNode(key, preds, succs);
  if (found != -1) return 0;
  node* add = new node();
  add->key = key;
  int topLevel = randomLevel(max_level);
  add->topLevel = topLevel;
  for (int level = 0; level< add->topLevel; level++) {
	for (int k = 0; k < FANOUT; k++) {
	  add->nexts[level][k].nxt = succs[level][k];
	  add->nexts[level][k].prefix = succs[level][k]->key;
	  if (preds[level][k]) {
		// scoot rest down
		node* pr = preds[level][k];
		for (int j = FANOUT-2; j >= k; j--) {
		  preds[level][k]->nexts[level][j+1].nxt = pr->nexts[level][j].nxt;
		  preds[level][k]->nexts[level][j+1].prefix = pr->nexts[level][j].prefix;
		}
		preds[level][k]->nexts[level][k].nxt = add;
		preds[level][k]->nexts[level][k].prefix  = key;
	  }
	}
  }
  return 1;
}

SkipList* SkipList::init_list(int sz, int high, int max_level) {
  SkipList* skip = new SkipList(max_level);
  int count = 0;
  while (count < sz) {
	if (skip->insert(rand() % high) > 0) {
	  count++;
	}
  }
  return skip;
}

SkipList* perfect_list(int sz, int max_level) {
  node** sk;
  sk = new node* [(unsigned long long)sz+2];
  for (int i = 0; i < sz+2; i++) {
	sk[i] = new node;
  }

  // set up head
  sk[0]->key = -1;
  sk[0]->topLevel = max_level;
  for (int i = 0; i < max_level; i++) {
	for (int k = 0; k < FANOUT; k++) {
	  sk[0]->nexts[i][k].prefix = k;
	  sk[0]->nexts[i][k].nxt = sk[1+k];
	}
  }
  for (int i = 1; i < max_level; i++) {
	for (int j = 0; j < FANOUT; j++) {
	  int ptr = int(pow(FANOUT, i)*(j+1)) - 1; // i=1 ptr=3, 7, 11, 15 
	  if (ptr < sz) {
		sk[0]->nexts[i][j].prefix = ptr;     // i=2 ptr=15, 31, 47, 63
		sk[0]->nexts[i][j].nxt = sk[ptr+1]; // i=3 ptr=63, ....
	  } else {
		sk[0]->nexts[i][j].prefix = INT_MAX;
		sk[0]->nexts[i][j].nxt = sk[sz+1];
	  }
	}
  }

  for (int i = 1; i < sz+1; i++) {
	sk[i]->key = i-1;
	sk[i]->topLevel = 1;
	for (int k = 2; k < max_level+1; k++) {
	  if (i % int(pow(FANOUT, k-1)) == 0) {
		sk[i]->topLevel = k;
	  }	
	}
	// fix up next when near end for +1 level
	for (int k = 0; k < FANOUT; k++) {
	  int np = sk[i]->key + 1 + k;
	  node* nptr = sk[i+1+k];
	  if (np >= sz) {
		np = INT_MAX;
		nptr = sk[sz+1];
	  }
	  sk[i]->nexts[0][k].prefix = np;
	  sk[i]->nexts[0][k].nxt = nptr;
	}

	for (int j = 1; j < sk[i]->topLevel; j++) {
	  for (int k = 0; k < FANOUT; k++) {
		int nextp = int(pow(FANOUT, j)*(k+1)) + i - 1;
		node* nextptr;;
		if (nextp >= sz) {
		  nextp = INT_MAX;
		  nextptr = sk[sz+1];
		} else {
		  nextptr = sk[nextp+1];
		}
		sk[i]->nexts[j][k].prefix = nextp;
		sk[i]->nexts[j][k].nxt = nextptr;
	  }
	}
	int last = sk[i]->topLevel-1;
	for (int k = 0; k < FANOUT; k++) {
	  if (sk[i]->nexts[last][k].prefix >= sz) {
		sk[i]->nexts[last][k].prefix = INT_MAX;
		sk[i]->nexts[last][k].nxt = sk[sz+1];
	  }
	}
  }

  for (int j = 0; j < sk[sz]->topLevel; j++) {
	for (int k = 0; k < FANOUT; k++) {
	  sk[sz]->nexts[j][k].prefix = INT_MAX;
	  sk[sz]->nexts[j][k].nxt = sk[sz+1];
	}
  }
  // set up tail
  sk[sz+1]->key = INT_MAX;
  sk[sz+1]->topLevel = max_level;
  for (int j = 0; j < max_level; j++) {
	for (int k = 0; k < FANOUT; k++) {
	  sk[sz+1]->nexts[j][k].prefix = INT_MAX;
	  sk[sz+1]->nexts[j][k].nxt = sk[sz+1];
	}
  }

  SkipList* sk4 = new SkipList;
  sk4->head = sk[0];
  sk4->tail = sk[sz+1];
  sk4->max_level = max_level;
  return sk4;
}

void SkipList::printlist() {
  node* ptr = head;
  while (ptr != tail) {
	printf("[K:%02d ", ptr->key);
	for (int level = 0; level < ptr->topLevel; level++) {
	  printf("N: ");
	  for (int k = 0; k < FANOUT; k++) {
		if (ptr->nexts[level][k].prefix == INT_MAX) {
		  printf("ND ");
		} else {		
		  printf("%02d ", ptr->nexts[level][k].nxt->key);
		}
		//printf("%02d ", ptr->nexts[level][k].nxt->key);
	  }
	}
	printf("]\n");
	ptr = ptr->nexts[0][0].nxt;
  }
}

void SkipList::pretty_print_skiplist() {
  node* ptr = head;
  while (ptr != 0) {
	if (ptr != head) {
	  for (int i = 0; i < ptr->topLevel; i++) {
		printf("   V   ");
	  }
	  for (int i = ptr->topLevel; i < max_level; i++) {
		printf("   |   ");
	  }
	}
	printf("\n");
	if (ptr == tail) {
	  for (int i = 0; i < max_level; i++) {
		printf("[K:ND] ");
	  }
	  printf("\n");
	  return;
	}
	for (int i = 0; i < ptr->topLevel; i++) {
	  printf("[K:%02d] ", ptr->key);
	}
	for (int i = ptr->topLevel; i < max_level; i++) {
	  printf("   |   ");
	}
	printf("\n");
	
	for (int i = 0; i < ptr->topLevel; i++) {
	  printf("[P:");
	  if (ptr->nexts[i][0].prefix == INT_MAX) {
		printf("ND");
	  } else {		
		printf("%02d", ptr->nexts[i][0].prefix);
	  }
	  printf("] ");
	}
	for (int i = ptr->topLevel; i < max_level; i++) {
	  printf("   |   ");
	}
	printf("\n");
	for (int i = 0; i < max_level; i++) {
	  printf("   |   ");
	}
	printf ("\n");
	ptr = ptr->nexts[0][0].nxt;
  }
}

void fillRandom(SkipList* skip, int sz, int high) {
  int count = 0;
  while (count < sz) {
	if (skip->insert(rand() % high) > 0) {
	  count++;
	}
  }
  printf("Inserted %d keys\n", count);
}

void fillForward(SkipList* skip, int sz) {
  int count = 0;
  while (count < sz) {
	if (skip->insert(count) > 0) {
	  count++;
	}
  }
  printf("Inserted %d keys\n", count);
}

void fillBackward(SkipList* skip, int sz) {
  int count = 0;
  while (count < sz) {
	if (skip->insert(sz-count-1) > 0) {
	  count++;
	}
  }
  printf("Inserted %d keys\n", count);
}

SkipList* empty_fill(int sz) {
  int high = 100;
  int maxl = floor(log(sz)/log(FANOUT)) + 1;
  SkipList* skip = new SkipList(maxl);
  fillBackward(skip, sz);
  return skip;
}

int perfect_insert_test(SkipList* sk) {
  assert(sk->lookup(99) <= 0);
  assert(sk->insert(99) > 0);
  sk->pretty_print_skiplist();
  sk->printlist();
  assert(sk->lookup(99) > 0);
  assert(sk->insert(99) <= 0);

  assert(sk->lookup(98) <= 0);
  assert(sk->insert(98) > 0);
  assert(sk->lookup(98) > 0);
  assert(sk->insert(98) <= 0);

  assert(sk->insert(90) > 0);
  assert(sk->lookup(90) > 0);

  sk->pretty_print_skiplist();
  sk->printlist();

  assert(sk->insert(97) > 0);
  assert(sk->lookup(97) > 0);
  sk->pretty_print_skiplist();
  sk->printlist();
}

int insert_test(int sz) {
  printf("Testing inserts...\n");
  SkipList* sk4 = empty_fill(sz);
  for (int i = 0; i < sz; i++) {
	assert(sk4->lookup(i) > 0);
  }
  printf("PASSED\n");
}

int basic_test(int sz) {
  printf("Testing perfect skiplist lookups...\n");
  SkipList* sk = perfect_list(sz, MAX_LEVEL);
  for (int i = 0; i < sz; i++) {
	assert (sk->lookup(i) > 0);
  }
  assert (sk->lookup(99) <= 0);
  printf("PASSED\n");
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = 16;
  if (argc > 1) {
	LIST_SIZE = atoi(argv[1]);
  }
  int POOL = 3*LIST_SIZE;
  int ITERATIONS = 10000;
  int maxl = floor(log(LIST_SIZE)/log(4))+1;
  if (LIST_SIZE < 99) {	
	basic_test(LIST_SIZE);
	insert_test(LIST_SIZE);
	exit(1);
  }
  SkipList* stest2 = SkipList::init_list(LIST_SIZE, POOL, maxl);
  assert(stest2);
  stest2->enable_counts();
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t start = ts.tv_sec*1000000000 + ts.tv_nsec;
  for (int i = 0; i < ITERATIONS; i++) {
	int key = (rand() % LIST_SIZE);
	if (ITERATIONS < 50) printf("key: %d\n", key);
	stest2->lookup(key);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  stest2->disable_counts();
  time_t lookup_time = ts.tv_sec*1000000000 + ts.tv_nsec;
  for (int i = LIST_SIZE; i < LIST_SIZE+ITERATIONS; i++) {
	stest2->insert(rand() % POOL);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t insert_time = ts.tv_sec*1000000000 + ts.tv_nsec;

  printf("insertns: %ld; lookupns: %ld; itr: %d; size: %d; levels: %d; probability: %d; ptr: %d\n", 
		 (insert_time-lookup_time)/(ITERATIONS), (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, maxl, FANOUT, stest2->get_ptr_count());
}

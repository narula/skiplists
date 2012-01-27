#include "skiplist-fanout.h"
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

SkipList::SkipList(int prob, int maxl) {
  probability = prob;
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::lookup(int key) {
  node *preds[max_level][FANOUT], *succs[max_level][FANOUT];
  if (findNode(key, preds, succs) >= 0) {
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

int SkipList::findNode(int key, node* preds[][FANOUT], node* succs[][FANOUT]) {
  node* pred = head;
  int lFound = -1;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node* curr;
	curr = pred->nexts[level][0].nxt;
	int ahead = 0;
	while (key > pred->nexts[level][ahead].prefix) {
	  ahead++;
	  curr = pred->nexts[level][ahead].nxt;
	  inc();
	}
	while (key > pred->nexts[level][0].prefix) {
	  pred = curr;
	  curr = pred->nexts[level][FANOUT-1].nxt;
	  inc();
	}
	preds[level][FANOUT-1] = pred;
	succs[level][FANOUT-1] = curr;
	if (lFound == -1 && key == pred->nexts[level][FANOUT-1].prefix) {
	  lFound = level;
	  continue;
	}

	// pred.key is less than key, curr.key is greater than key.
	int k = 0;
	while (key > pred->nexts[level][k].prefix) {
	  k++;
	}
	//assert(k < FANOUT);
	if (k != 0) {
	  if (lFound == -1 && key == pred->nexts[level][k-1].prefix) {
		lFound = level;
	  }
	  pred = pred->nexts[level][k-1].nxt;
	  inc();
	}
  }
  return lFound;
}

// Doesn't work.
int SkipList::insert(int key) {
  node *preds[max_level][FANOUT], *succs[max_level][FANOUT];
  int found = findNode(key, preds, succs);
  if (found != -1) return 0;
  node* add = new node();
  add->key = key;
  int topLevel = randomLevel(max_level);
  for (int i = 0; i < topLevel; i++) {
	add->nexts[i][FANOUT-1].nxt = succs[i][FANOUT-1];
	add->nexts[i][FANOUT-1].prefix = succs[i][FANOUT-1]->key;
	preds[i][FANOUT-1]->nexts[i][FANOUT-1].nxt = add;
	preds[i][FANOUT-1]->nexts[i][FANOUT-1].prefix = key;
	for (int k = 0; k < FANOUT-1; k++) {
	  if (preds[i][FANOUT-1]->nexts[i][k].prefix > key) {
		add->nexts[i][k].nxt = preds[i][FANOUT-1]->nexts[i][k].nxt;
		add->nexts[i][k].prefix = preds[i][FANOUT-1]->nexts[i][k].prefix;
		preds[i][k]->nexts[i][k].nxt = NULL;
		preds[i][k]->nexts[i][k].prefix = 0;
	  }
	}
  }
  return 1;
}

SkipList* SkipList::init_list(int sz, int max_level) {
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
	  sk[0]->nexts[i][k].prefix = 0;
	  sk[0]->nexts[i][k].nxt = sk[1];
	}
  }
  for (int i = 1; i < max_level; i++) {
	for (int j = 0; j < FANOUT; j++) {
	  int ptr = int(pow(FANOUT, i-1)*(j+1)) - 1; // i=1 ptr=3, 7, 11, 15 
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
	int np = sk[i]->key+1;
	node* nptr = sk[i+1];
	if (np >= sz) {
	  np = INT_MAX;
	  nptr = sk[sz+1];
	}
	sk[i]->nexts[0][FANOUT-1].prefix = np;
	sk[i]->nexts[0][FANOUT-1].nxt = nptr;

	for (int j = 1; j < sk[i]->topLevel; j++) {
	  for (int k = 0; k < FANOUT; k++) {
		int nextp = int(pow(FANOUT, j-1)*(k+1)) + i - 1;
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

int insert_test(SkipList* sk) {
  assert (sk->lookup(99) <= 0);
  assert (sk->insert(99) > 0);
  assert (sk->lookup(99) > 0);
  assert (sk->insert(99) <= 0);
}

int basic_test(SkipList* sk) {
  for (int i = 0; i < 16; i++) {
	assert (sk->lookup(i) > 0);
  }
  assert (sk->lookup(99) <= 0);
  printf("PASSED\n");
}

void SkipList::printlist() {
  node* ptr = head;
  while (ptr != tail) {
	printf("[K:%02d ", ptr->key);
	printf("N: %02d ", ptr->nexts[0][0].nxt->key);
	for (int j = 1; j < ptr->topLevel; j++) {
	  printf("N: ");
	  for (int k = 0; k < FANOUT; k++) {
		printf("%02d ", ptr->nexts[j][k].nxt->key);
	  }
	}
	printf("]\n");
	ptr = ptr->nexts[0][0].nxt;
  }
  printf("]\n");
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
	  if (ptr->nexts[i][FANOUT-1].prefix == INT_MAX) {
		printf("ND");
	  } else {		
		printf("%02d", ptr->nexts[i][FANOUT-1].prefix);
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
	ptr = ptr->nexts[0][FANOUT-1].nxt;
  }
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int ITERATIONS = 10000;
  int maxl = floor(log(LIST_SIZE)/log(4))+1;
  SkipList* stest2 = SkipList::init_list(LIST_SIZE, maxl);
  assert(stest2);
  if (LIST_SIZE < 99) {
	basic_test(stest2);
	stest2->pretty_print_skiplist();
  }
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
  printf("lookup: %ld; itr: %d; size: %d; levels: %d; probability: %d; ptr: %d\n", 
		 (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, maxl, FANOUT, stest2->get_ptr_count());
}

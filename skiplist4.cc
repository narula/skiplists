#include "skiplist4.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>


SkipList4::SkipList4() {
}

SkipList4::SkipList4(int prob, int maxl) {
  head = new node4();
  tail = new node4();
  probability = prob;
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
  head->key = -1;
  head->topLevel = max_level;
  tail->key = INT_MAX;
  tail->topLevel = max_level;
  for (int i = 0; i < MAX_LEVEL; i++) {
	for (int j = 0; i < 4; i++) {
	  head->nexts[j][i].nxt = tail;
	  head->nexts[j][i].prefix = INT_MAX;
	}
  }
}

SkipList4::~SkipList4() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList4::lookup(int key) {
  node4* preds[max_level];
  node4* succs[max_level];
  if (findNode(key, preds, succs) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

// array laid out [ 0   1     2   ..
// array laid out [ top top-1 top-2
int alevel(int logical_level, node4* node) {
  assert(logical_level < node->topLevel);
  //  int actual_level = node->topLevel - 1 - logical_level;
  return logical_level;
  //  return actual_level;
}

int SkipList4::findNode(int key, node4* preds[], node4* succs[]) {
  int lFound = -1;
  node4* pred = head;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node4* curr = pred->nn(level, 3)->nxt;
	while (key > pred->nn(level, 3)->prefix) {
	  int idx = alevel(level, curr);
	  pred = curr; 
	  curr = pred->nexts[3][idx].nxt;
	}
	if (lFound == -1 && key == curr->key) {
	  lFound = level;
	}
	// reversed for ease
	preds[level] = pred;
	succs[level] = curr;
  }
  return lFound;
}

void SkipList4::pretty_print_skiplist() {
  node4* ptr = head;
  SkipList4* sk4 = new SkipList4();
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
	  if (ptr->nn(i, 3)->prefix == INT_MAX) {
		printf("[P:ND] ");
	  } else {
		printf("[P:%02d] ", ptr->nn(i, 3)->prefix);
	  }
	}
	for (int i = ptr->topLevel; i < max_level; i++) {
	  printf("   |   ");
	}
	printf("\n");
	for (int i = 0; i < max_level; i++) {
	  printf("   |   ");
	}
	printf ("\n");
	ptr = ptr->nn(0, 3)->nxt;
  }
}

SkipList4* SkipList4::init_list(int sz) {
  struct node4* sk;
  sk = new node4[sz+2];

  // set up head
  sk[0].key = -1;
  sk[0].topLevel = MAX_LEVEL;
  sk[0].nexts[3][0].prefix = 0;
  sk[0].nexts[3][0].nxt = &(sk[1]);
  for (int i = 1; i < MAX_LEVEL; i++) {
	for (int j = 1; j <= 4; j++) {
	  int ptr = int(pow(4, i-1)*j) - 1;
	  sk[0].nexts[j-1][i].prefix = ptr;
	  sk[0].nexts[j-1][i].nxt = &(sk[ptr]);
	}
  }

  for (int i = 1; i < sz+1; i++) {
	sk[i].key = i-1;
	sk[i].topLevel = 1;
	for (int k = 2; k < MAX_LEVEL; k++) {
	  if (i % int(pow(4, k-1)) == 0) {
		sk[i].topLevel = k;
	  }	
	}
	// fix up next when near end for +1 level
	int np = sk[i].key+1;
	node4* nptr = &(sk[i+1]);
	if (np >= sz) {
	  np = INT_MAX;
	  nptr = &(sk[sz+1]);
	}
	sk[i].nexts[3][0].prefix = np;
	sk[i].nexts[3][0].nxt = nptr;

	for (int j = 1; j < sk[i].topLevel; j++) {
	  for (int k = 1; k <= 4; k++) {
		int nextp = int(pow(4, j-1)*k) + i - 1;
		node4* nextptr = &(sk[nextp]);
		if (nextp >= sz) {
		  nextp = INT_MAX;
		  nextptr = &(sk[sz+1]);
		}
		sk[i].nexts[k][j].prefix = nextp;
		sk[i].nexts[k][j].nxt = nextptr;
	  }
	}
	int last = sk[i].topLevel-1;
	for (int k = 0; k < 4; k++) {
	  if (sk[i].nexts[k][last].prefix >= sz) {
		sk[i].nexts[k][last].prefix = INT_MAX;
		sk[i].nexts[k][last].nxt = &(sk[sz+1]);
	  }
	}
  }

  // set up tail
  sk[sz+1].key = INT_MAX;
  sk[sz+1].topLevel = MAX_LEVEL;

  SkipList4* sk4 = new SkipList4;
  sk4->head = &sk[0];
  sk4->tail = &sk[sz+1];
  sk4->max_level = MAX_LEVEL;
  return sk4;
}

int basic_test(SkipList4* sk) {
  assert (sk->lookup(5) <= 0);
}

void printlist(SkipList4* sk, int sz) {
  node4* start = sk->head;
  for (int i = 0; i < sz+2; i++) {
	printf("[K:%02d ", start[i].key);
	for (int j = 0; j < start[i].topLevel; j++) {
	  printf("N:%02d ", start[i].nexts[3][j].prefix);
	}
	printf("]\n");
  }
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int ITERATIONS = 10000;
  SkipList4* stest2 = SkipList4::init_list(LIST_SIZE);
  assert(stest2);
  printlist(stest2, LIST_SIZE);
  basic_test(stest2);
}

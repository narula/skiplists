#include "skiplist4.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>


SkipList4::SkipList4() {
  count = 0;
  pointer_follows = 0;
}

SkipList4::SkipList4(int prob, int maxl) {
  probability = prob;
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
}

SkipList4::~SkipList4() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList4::lookup(int key) {
  node4* preds[max_level][4];
  node4* succs[max_level][4];
  if (findNode(key, preds, succs) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

int SkipList4::findNode(int key, node4* preds[][4], node4* succs[][4]) {
  int lFound = -1;
  node4* pred = head;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node4* curr;
	curr = pred->nexts[level][3].nxt;
	while (key > pred->nexts[level][3].prefix) {
	  pred = curr;
	  curr = pred->nexts[level][3].nxt;
	  inc();
	}
	if (lFound == -1 && key == curr->key) {
	  return level;
	}

	// pred.key is less than key, curr.key is greater than key.
	int k = 0;
	while (key > pred->nexts[level][k].prefix) {
	  k++;
	}
	assert(k < 4);
	if (k != 0) {
	  pred = pred->nexts[level][k-1].nxt;
	}
	if (lFound == -1 && key == pred->key) {
	  return level;
	}
  }
  return lFound;
}

SkipList4* SkipList4::init_list(int sz, int max_level) {
  node4** sk;
  sk = new node4* [(unsigned long long)sz+2];
  for (int i = 0; i < sz+2; i++) {
	sk[i] = new node4;
  }

  // set up head
  sk[0]->key = -1;
  sk[0]->topLevel = max_level;
  for (int i = 0; i < max_level; i++) {
	for (int k = 0; k < 4; k++) {
	  sk[0]->nexts[i][k].prefix = 0;
	  sk[0]->nexts[i][k].nxt = sk[1];
	}
  }
  for (int i = 1; i < max_level; i++) {
	for (int j = 0; j < 4; j++) {
	  int ptr = int(pow(4, i-1)*(j+1)) - 1; // i=1 ptr=3, 7, 11, 15 
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
	  if (i % int(pow(4, k-1)) == 0) {
		sk[i]->topLevel = k;
	  }	
	}
	// fix up next when near end for +1 level
	int np = sk[i]->key+1;
	node4* nptr = sk[i+1];
	if (np >= sz) {
	  np = INT_MAX;
	  nptr = sk[sz+1];
	}
	sk[i]->nexts[0][3].prefix = np;
	sk[i]->nexts[0][3].nxt = nptr;

	for (int j = 1; j < sk[i]->topLevel; j++) {
	  for (int k = 0; k < 4; k++) {
		int nextp = int(pow(4, j-1)*(k+1)) + i - 1;
		node4* nextptr;;
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
	for (int k = 0; k < 4; k++) {
	  if (sk[i]->nexts[last][k].prefix >= sz) {
		sk[i]->nexts[last][k].prefix = INT_MAX;
		sk[i]->nexts[last][k].nxt = sk[sz+1];
	  }
	}
  }

  for (int j = 0; j < sk[sz]->topLevel; j++) {
	for (int k = 0; k < 4; k++) {
	  sk[sz]->nexts[j][k].prefix = INT_MAX;
	  sk[sz]->nexts[j][k].nxt = sk[sz+1];
	}
  }
  // set up tail
  sk[sz+1]->key = INT_MAX;
  sk[sz+1]->topLevel = max_level;
  for (int j = 0; j < max_level; j++) {
	for (int k = 0; k < 4; k++) {
	  sk[sz+1]->nexts[j][k].prefix = INT_MAX;
	  sk[sz+1]->nexts[j][k].nxt = sk[sz+1];
	}
  }


  SkipList4* sk4 = new SkipList4;
  sk4->head = sk[0];
  sk4->tail = sk[sz+1];
  sk4->max_level = max_level;
  return sk4;
}

int basic_test(SkipList4* sk) {
  assert (sk->lookup(5) > 0);
  assert (sk->lookup(99) <= 0);
  printf("PASSED\n");
}

void SkipList4::printlist() {
  node4* ptr = head;
  while (ptr != tail) {
	printf("[K:%02d ", ptr->key);
	printf("N: %02d ", ptr->nexts[0][0].nxt->key);
	for (int j = 1; j < ptr->topLevel; j++) {
	  printf("N: ");
	  for (int k = 0; k < 4; k++) {
		printf("%02d ", ptr->nexts[j][k].nxt->key);
	  }
	}
	printf("]\n");
	ptr = ptr->nexts[0][0].nxt;
  }
  printf("]\n");
}

void SkipList4::pretty_print_skiplist() {
  node4* ptr = head;
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
	printf("[P:%02d] ", ptr->nexts[0][3].prefix);
	
	for (int i = 1; i < ptr->topLevel; i++) {
	  printf("[P:");
	  if (ptr->nexts[i, 3]->prefix == INT_MAX) {
		printf("ND");
	  } else {		
		printf("%02d", ptr->nexts[i][3].prefix);
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
	ptr = ptr->nexts[0][3].nxt;
  }
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int ITERATIONS = 10000;
  int maxl = floor(log(LIST_SIZE)/log(4))+1;
  SkipList4* stest2 = SkipList4::init_list(LIST_SIZE, maxl);
  assert(stest2);
  if (LIST_SIZE < 99) {
	//stest2->printlist();
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
		 (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, maxl, 4, stest2->get_ptr_count());
}

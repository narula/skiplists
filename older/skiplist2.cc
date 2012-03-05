#include "skiplist2.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>


SkipList2::SkipList2() {
}

SkipList2::SkipList2(int prob, int maxl) {
  probability = prob;
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
}

SkipList2::~SkipList2() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList2::lookup(int key) {
  node2* preds[max_level];
  node2* succs[max_level];
  if (findNode(key, preds, succs) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

int SkipList2::findNode(int key, node2* preds[], node2* succs[]) {
  node2* pred = head;
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node2* tmp;
	node2* curr = pred->nexts[level].nxt;
	assert(curr->topLevel >= level);
	while (key > pred->nexts[level].prefix) {
	  tmp = pred;
	  pred = curr;
	  curr = pred->nexts[level].nxt;
	  inc();
	}
	if (key == pred->nexts[level].prefix) {
	  return level;
	}
  }
  return -1;
}

SkipList2* SkipList2::init_list(int sz, int max_level) {
  node2** sk;
  sk = new node2* [(unsigned long long)sz+2];
  for (int i = 0; i < sz+2; i++) {
	sk[i] = new node2;
  }

  // set up head
  sk[0]->key = -1;
  sk[0]->topLevel = max_level;
  for (int i = 0; i < max_level; i++) {
	sk[0]->nexts[i].prefix = 0;
	sk[0]->nexts[i].nxt = sk[1];
  }
  for (int i = 1; i < max_level; i++) {
	int ptr = int(pow(2, i)) - 1; // i=1 ptr=1, 3, 5, 7 
	if (ptr < sz) {
	  sk[0]->nexts[i].prefix = ptr;     // i=2 ptr=3, 7, 11, 15
	  sk[0]->nexts[i].nxt = sk[ptr+1]; // i=3 ptr=7, 15, 23, 31
	} else {
	  sk[0]->nexts[i].prefix = INT_MAX;
	  sk[0]->nexts[i].nxt = sk[sz+1];
	}
  }

  for (int i = 1; i < sz+1; i++) {
	sk[i]->key = i-1;
	sk[i]->topLevel = 1;
	for (int k = 2; k < max_level+1; k++) {
	  if (i % int(pow(2, k-1)) == 0) {
		sk[i]->topLevel = k;
	  }	
	}
	// fix up next when near end for +1 level
	int np = sk[i]->key+1;
	node2* nptr = sk[i+1];
	if (np >= sz) {
	  np = INT_MAX;
	  nptr = sk[sz+1];
	}
	sk[i]->nexts[0].prefix = np;
	sk[i]->nexts[0].nxt = nptr;

	for (int j = 1; j < sk[i]->topLevel; j++) {
	  int nextp = int(pow(2, j)) + i - 1;
	  node2* nextptr;;
	  if (nextp >= sz) {
		nextp = INT_MAX;
		nextptr = sk[sz+1];
	  } else {
		nextptr = sk[nextp+1];
	  }
	  sk[i]->nexts[j].prefix = nextp;
	  sk[i]->nexts[j].nxt = nextptr;
	}
	int last = sk[i]->topLevel-1;
	if (sk[i]->nexts[last].prefix >= sz) {
	  sk[i]->nexts[last].prefix = INT_MAX;
	  sk[i]->nexts[last].nxt = sk[sz+1];
	}
  }

  for (int j = 0; j < sk[sz]->topLevel; j++) {
	sk[sz]->nexts[j].prefix = INT_MAX;
	sk[sz]->nexts[j].nxt = sk[sz+1];
  }

  // set up tail
  sk[sz+1]->key = INT_MAX;
  sk[sz+1]->topLevel = max_level;
  for (int j = 0; j < max_level; j++) {
	sk[sz+1]->nexts[j].prefix = INT_MAX;
	sk[sz+1]->nexts[j].nxt = sk[sz+1];
  }

  SkipList2* sk4 = new SkipList2;
  sk4->head = sk[0];
  sk4->tail = sk[sz+1];
  sk4->max_level = max_level;
  return sk4;
}

int basic_test(SkipList2* sk) {
  assert (sk->lookup(5) > 0);
  assert (sk->lookup(99) <= 0);
  printf("PASSED\n");
}

void SkipList2::printlist() {
  node2* ptr = head;
  while (ptr != tail) {
	printf("[K:%02d ", ptr->key);
	printf("N: %02d ", ptr->nexts[0].nxt->key);
	for (int j = 1; j < ptr->topLevel; j++) {
	  printf("N: ");
	  printf("%02d ", ptr->nexts[j].nxt->key);
	}
	printf("]\n");
	ptr = ptr->nexts[0].nxt;
  }
  printf("]\n");
}

void SkipList2::pretty_print_skiplist() {
  node2* ptr = head;
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
	  if (ptr->nexts[i].prefix == INT_MAX) {
		printf("ND");
	  } else {
		printf("%02d", ptr->nexts[i].prefix);
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
	ptr = ptr->nexts[0].nxt;
  }
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int ITERATIONS = 10000;
  int maxl = floor(log(LIST_SIZE)/log(2))+1;
  if (maxl > MAX_LEVEL) maxl = MAX_LEVEL;
  SkipList2* stest2 = SkipList2::init_list(LIST_SIZE, maxl);
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
	stest2->lookup(rand() % LIST_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  stest2->disable_counts();
  time_t lookup_time = ts.tv_sec*1000000000 + ts.tv_nsec;
  printf("lookup: %ld; itr: %d; size: %d; levels: %d; probability: %d; ptr: %d\n", 
		 (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, maxl, 2, stest2->get_ptr_count());
}

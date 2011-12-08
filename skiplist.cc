#include "skiplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>

node* new_node() {
  node* nnode = new node();
  // for (int i = 0; i < MAX_LEVEL; i++) {
  // 	nnode->next[i] = 0;
  // 	nnode->prefix[i] = 0;
  // }
  return nnode;
}

SkipList::SkipList(int prob, int sz) {
  head = new_node();
  tail = new_node();
  probability = prob;
  max_level = floor(log(sz)/log(probability));
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
  head->key = -1;
  head->topLevel = MAX_LEVEL;
  tail->key = INT_MAX;
  tail->topLevel = MAX_LEVEL;
  for (int i = 0; i < MAX_LEVEL; i++) {
	head->nexts[i].nxt = tail;
	head->nexts[i].prefix = INT_MAX;
  }
}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::findNode(int key, node* preds[], node* succs[]) {
  int lFound = -1;
  node* pred = head;
  for (int level = max_level-1; level >= 0; level--) {
	node* curr = pred->nexts[level].nxt;
	while (key > pred->nexts[level].prefix) {
	  pred = curr; 
	  curr = pred->nexts[level].nxt;
	}
	if (lFound == -1 && key == curr->key) {
	  lFound = level;
	}
	preds[level] = pred;
	succs[level] = curr;
  }
  return lFound;
}

int SkipList::lookup(int key) {
  node* preds[MAX_LEVEL];
  node* succs[MAX_LEVEL];
  if (findNode(key, preds, succs) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

int randomLevel(int max, int probability) {
  unsigned long long x = rand();
  int layer = 0;
  switch (probability) {
  case 4:
	while((x & 3) == 3){
	  layer += 1;
	  x >>= 2;
	}
	break;
  case 8:
	while((x & 7) == 7){
	  layer += 1;
	  x >>= 3;
	}
	break;
  case 16:
	while((x & 15) == 15){
	  layer += 1;
	  x >>= 4;
	}
	break;
  case 32:
	while((x & 31) == 31){
	  layer += 1;
	  x >>= 5;
	}
	break;
  default:
	while((x & 1) != 0) {
	  layer += 1;
	  x >>= 1;
	}
	break;
  }
  if(layer >= max)
	layer = max - 1;
  return layer;
}

int SkipList::insert(int key) {
  node *preds[MAX_LEVEL], *succs[MAX_LEVEL];
  int lFound = findNode(key, preds, succs);
  if (lFound != -1) {
	node* found = succs[lFound];
	return 0;
  }
  node* add = new_node();
  add->key = key;
  int topLevel = randomLevel(max_level, probability);
  add->topLevel = topLevel+1;
  for (int i = 0; i <= topLevel; i++) {
	add->nexts[i].nxt = succs[i];
	add->nexts[i].prefix = succs[i]->key;
	preds[i]->nexts[i].nxt = add;
	preds[i]->nexts[i].prefix = key;
  }
  return 1;
}

void SkipList::print_skiplist() {
  node* ptr = head;
  int i = 0;
  while (ptr != 0) {
	printf("[N:%04d K:%04d ML:%d]  ", i, ptr->key, ptr->topLevel);
	i++;
	ptr = ptr->nexts[0].nxt;
  }
}

void SkipList::pretty_print_skiplist() {
  node* ptr = head;
  while (ptr != 0) {
	if (ptr != head) {
	  for (int i = 0; i < ptr->topLevel; i++) {
		printf("   V   ");
	  }
	  for (int i = ptr->topLevel; i < MAX_LEVEL; i++) {
		printf("   |   ");
	  }
	}
	printf("\n");
	if (ptr == tail) {
	  for (int i = 0; i < MAX_LEVEL; i++) {
		printf("[K:ND] ");
	  }
	  printf("\n");
	  return;
	}
	for (int i = 0; i < ptr->topLevel; i++) {
	  printf("[K:%02d] ", ptr->key);
	}
	for (int i = ptr->topLevel; i < MAX_LEVEL; i++) {
	  printf("   |   ");
	}
	printf("\n");
	for (int i = 0; i < ptr->topLevel; i++) {
	  if (ptr->nexts[i].prefix == INT_MAX) {
		printf("[P:ND] ");
	  } else {
		printf("[P:%02d] ", ptr->nexts[i].prefix);
	  }
	}
	for (int i = ptr->topLevel; i < MAX_LEVEL; i++) {
	  printf("   |   ");
	}
	printf("\n");
	for (int i = 0; i < MAX_LEVEL; i++) {
	  printf("   |   ");
	}
	printf ("\n");
	ptr = ptr->nexts[0].nxt;
  }
}

SkipList* init_list(int sz, int high, int probability) {
  SkipList* skip = new SkipList(probability, sz);
  int ret = 0;
  int count = 0;
  while (count < sz) {
	ret = skip->insert(rand() % high);
	if (ret > 0) {
	  count++;
	}
  }
  return skip;
}

int main(int argc, char** argv) {
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int PROBABILITY = atoi(argv[2]);
  int POOL = 3*LIST_SIZE;
  int ITERATIONS = 10000;
  int seconds = 10;
  SkipList* stest2 = init_list(LIST_SIZE, POOL, PROBABILITY);
  assert(stest2);
  timespec ts;
  if (LIST_SIZE < 100) {
	stest2->pretty_print_skiplist();
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t start = ts.tv_sec*1000000000 + ts.tv_nsec;
  for (int i = 0; i < ITERATIONS; i++) {
	stest2->lookup(rand() % POOL);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t lookup_time = ts.tv_sec*1000000000 + ts.tv_nsec;
  for (int i = LIST_SIZE; i < LIST_SIZE+ITERATIONS; i++) {
	stest2->insert(rand() % POOL);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t insert_time = ts.tv_sec*1000000000 + ts.tv_nsec;

  printf("insert: %ld; lookup: %ld; itr: %d; size: %d; levels: %d; probability: %d\n", 
		 (insert_time-lookup_time)/(ITERATIONS), (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, int(floor(log(LIST_SIZE)/log(PROBABILITY))), PROBABILITY);
}


// [DONE] Read-only not-concurrent skip list working
// [DONE] Measure perf
// [DONE] Verify O(logn) performance
// [DONE] bench.py to run with different sizes, write output.
// [DONE] verify log_2 performance
// re-order node array to move prefixes and next nodes together, benchmark
// log_4 algorithm read-only (not concurrent?) skip list working
// Fix amount of time, not amount of work.  setitimer.
// multi-threaded measure perf
// Concurrent skip list working

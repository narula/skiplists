#include "skiplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>

node* new_node() {
  node* nnode = new node();
  for (int i = 0; i < MAX_LEVEL; i++) {
	nnode->next[i] = 0;
	nnode->prefix[i] = 0;
  }
  return nnode;
}

SkipList::SkipList() {
  head = new_node();
  tail = new_node();
  head->key = -1;
  head->topLevel = MAX_LEVEL;
  tail->key = INT_MAX;
  tail->topLevel = MAX_LEVEL;
  for (int i = 0; i < MAX_LEVEL; i++) {
	head->next[i] = tail;
	head->prefix[i] = INT_MAX;
  }
}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::findNode(int key, node* preds[], node* succs[]) {
  int lFound = -1;
  node* pred = head;
  for (int level = MAX_LEVEL-1; level >= 0; level--) {
	node* curr = pred->next[level];
	while (key > pred->prefix[level]) {
	  pred = curr; 
	  curr = pred->next[level];
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

int randomLevel(int max) {
  int l = rand() % max;
  return l;
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
  int topLevel = randomLevel(MAX_LEVEL);
  add->topLevel = topLevel;
  for (int i = 0; i <= topLevel; i++) {
	add->next[i] = succs[i];
	add->prefix[i] = succs[i]->key;
	preds[i]->next[i] = add;
	preds[i]->prefix[i] = key;
  }
  return 1;
}

void SkipList::print_skiplist() {
  node* ptr = head;
  int i = 0;
  while (ptr != 0) {
	printf("[N:%04d K:%04d ML:%d]  ", i, ptr->key, ptr->topLevel);
	i++;
	ptr = ptr->next[0];
  }
}

void SkipList::pretty_print_skiplist() {
  node* ptr = head;
  while (ptr != 0) {
	for (int i = 0; i <= ptr->topLevel; i++) {
	  printf("[K:%04d] ", ptr->key);
	}
	printf("\n");
	for (int i = 0; i <= ptr->topLevel; i++) {
	  int prefix = 0;
	  if (ptr->next[i] != 0) {
		prefix = ptr->next[i]->key;
	  }
	  printf("[N:%04d] ", prefix);
	}
	printf("\n|\nV\n");
	ptr = ptr->next[0];
  }
}

SkipList* init_list(int sz, int high) {
  SkipList* skip = new SkipList();
  int ret = 0;
  for (int i = 0; i < sz; i++) {
	ret = skip->insert(rand() % high);
	if (ret < 0) {
	  printf("Problem! %d\n", i);
	  skip->pretty_print_skiplist();
	  return NULL;
	}
  }
  return skip;
}

int main(int argc, char** argv) {
  int LIST_SIZE = atoi(argv[1]);
  int POOL = 3*LIST_SIZE;
  int ITERATIONS = 10000;
  int seconds = 10;
  SkipList* stest2 = init_list(LIST_SIZE, POOL);
  assert(stest2);
  timespec ts;
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

  printf("Timing(microsec) insert: %ld, lookup: %ld, %d itr, %d size\n", 
		 (insert_time-lookup_time)/(1000*ITERATIONS), (lookup_time-start)/(1000*ITERATIONS), ITERATIONS, LIST_SIZE);
}


// Read-only not-concurrent skip list working
// Measure perf  --  fix amount of time, not amount of work.
// Concurrent skip list working
// multi-threaded measure perf
// log_4 algorithm read-only (not concurrent?) skip list working

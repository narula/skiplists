#include "skiplist.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <limits.h>

node* new_node() {
  node* nnode = new node();
  for (int i = 0; i < MAX_LEVEL; i++) {
	nextinfo* ni = new nextinfo();
	ni->next = 0;
	ni->prefix_hint = 0;
	nnode->nexts[i] = *ni;
  }
  return nnode;
}

SkipList::SkipList() {
  head = new_node();
  tail = new_node();
  head->key = -1;
  tail->key = INT_MAX;
  for (int i = 0; i < MAX_LEVEL; i++) {
	head->nexts[i].next = tail;
	head->nexts[i].prefix_hint = INT_MAX;
  }
}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::findNode(int key, node* preds[], node* succs[]) {
  int lFound = -1;
  node* pred = head;
  for (int l = MAX_LEVEL-1; l >= 0; l--) {
	// first time this is equal to tail
	node* curr = pred->nexts[l].next;
	// not
	while (key > pred->nexts[l].prefix_hint) {
	  pred = curr; 
	  curr = pred->nexts[l].next;
	}
	if (lFound == -1 && key == curr->key) {
	  lFound = l;
	}
	// pred is head
	// curr is tail
	preds[l] = pred;
	succs[l] = curr;
  }
  return lFound;
}

int SkipList::lookup(int key) {
  node* preds[MAX_LEVEL];
  node* succs[MAX_LEVEL];
  if (findNode(key, preds, succs) > 0) {
	return 1;
  } else {
	return 0;
  }
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
  for (int i = 0; i < MAX_LEVEL; i++) {
	add->nexts[i].next = succs[i];
	add->nexts[i].prefix_hint = succs[i]->key;
	preds[i]->nexts[i].next = add;
	preds[i]->nexts[i].prefix_hint = key;
  }
  return 1;
}

void SkipList::print_skiplist() {
  node* ptr = head;
  int i = 0;
  while (ptr != 0) {
	printf("Index: %d\t Key: %d\n", i, ptr->key);
	i++;
	ptr = ptr->nexts[0].next;
  }
}

int main(int argc, char** argv) {
  SkipList stest = SkipList();
  int ret = 0;
  for (int i = 0; i < 10; i++) {
	ret = stest.insert(i);
	if (ret != 1) {
	  printf("Problem!\n");
	}
  }
  stest.print_skiplist();
  for (int i = 0; i < 5; i++) {
	int value;
	ret = stest.lookup(i);
	if (ret != 1) {
	  printf("Problem!\n");
	}
  }
  int ITERATIONS = 100000;
  SkipList stest2 = SkipList();
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  time_t start = ts.tv_nsec;
  for (int i = 0; i < ITERATIONS; i++) {
	int ret = stest2.insert(i);
	assert(ret);
  }
  clock_gettime(CLOCK_REALTIME, &ts);
  time_t insert_time = ts.tv_nsec;
  for (int i = 0; i < ITERATIONS; i++) {
	int value;
	int ret = stest2.lookup(i);
	assert(ret);
  }
  clock_gettime(CLOCK_REALTIME, &ts);
  time_t lookup_time = ts.tv_nsec;
  printf("Timing(ns) insert: %ld, lookup: %ld, %d itr\n", 
		 (insert_time-start)/ITERATIONS, (lookup_time-insert_time)/ITERATIONS, ITERATIONS);
}

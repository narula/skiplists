#include "skiplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>


SkipList::SkipList(int prob, int maxl) {
  head = new node();
  tail = new node();
  probability = prob;
  max_level = maxl;
  if (max_level == 0) max_level = 1;
  if (max_level > MAX_LEVEL) max_level = MAX_LEVEL;
  head->key = -1;
  head->topLevel = max_level;
  tail->key = INT_MAX;
  tail->topLevel = max_level;
  for (int i = 0; i < MAX_LEVEL; i++) {
	head->nexts[i].nxt = tail;
	head->nexts[i].prefix = INT_MAX;
  }
  count = 0;
  pointer_follows = 0;
}

SkipList::~SkipList() {
  if (head) delete head;
  if (tail) delete tail;
}

int SkipList::lookup(int key) {
  node* preds[max_level];
  node* succs[max_level];
  if (findNode(key, preds, succs) >= 0) {
	return 1;
  } else {
	return 0;
  }
}

int randomLevel(int max, int probability) {
  unsigned long long x = rand();
  int toplayer = 1;
  switch (probability) {
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
  case 32:
	while((x & 31) == 31){
	  toplayer += 1;
	  x >>= 5;
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
// array laid out [ 0   1     2   ..
// array laid out [ top top-1 top-2
int alevel(int logical_level, node* node) {
  assert(logical_level < node->topLevel);
  int actual_level = node->topLevel - 1 - logical_level;
  //return actual_level;
  return logical_level;
}

int SkipList::findNode(int key, node* preds[], node* succs[]) {
  int lFound = -1;
  node* pred = head;
  inc();
  for (int level = pred->topLevel-1; level >= 0; level--) {
	node* curr = pred->nn(level)->nxt;
	//	while (key > pred->nn(level)->prefix) {
	int nxt_key = pred->nn(level)->prefix;
	while (key > nxt_key) {
	  int idx = alevel(level, curr);
	  pred = curr; 
	  curr = pred->nexts[idx].nxt;
	  inc();
	  nxt_key = pred->nn(level)->prefix;
	}
	if (lFound == -1 && key == nxt_key) {
	  lFound = level;
	}
	// reversed for ease
	preds[level] = pred;
	succs[level] = curr;
  }
  return lFound;
}

int SkipList::insert(int key) {
  node *preds[max_level], *succs[max_level];
  int lFound = findNode(key, preds, succs);
  if (lFound != -1) {
	node* found = succs[lFound];
	return 0;
  }
  node* add = new node();
  add->key = key;
  int topLevel = randomLevel(max_level, probability);
  add->topLevel = topLevel;
  for (int i = 0; i < topLevel; i++) {
	add->nn(i)->nxt = succs[i];
	add->nn(i)->prefix = succs[i]->key;
	preds[i]->nn(i)->nxt = add;
	preds[i]->nn(i)->prefix = key;
  }
  return 1;
}

void SkipList::print_skiplist() {
  node* ptr = head;
  int i = 0;
  while (ptr != 0) {
	printf("[N:%04d K:%04d ML:%d]  ", i, ptr->key, ptr->topLevel);
	i++;
	ptr = ptr->nn(i)->nxt;
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
	  if (ptr->nn(i)->prefix == INT_MAX) {
		printf("[P:ND] ");
	  } else {
		printf("[P:%02d] ", ptr->nn(i)->prefix);
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
	ptr = ptr->nn(0)->nxt;
  }
}

SkipList* SkipList::init_list(int sz, int high, int probability) {
  int maxl = floor(log(sz)/log(probability));
  SkipList* skip = new SkipList(probability, maxl);
  int count = 0;
  while (count < sz) {
	if (skip->insert(rand() % high) > 0) {
	  count++;
	}
  }
  return skip;
}

void basic_test() {
  SkipList* sk = SkipList::init_list(1000, 1000, 2);
  int error = 0;
  for (int i = 999; i >= 0; i--) {
	if (sk->lookup(i) <= 0) {
	  printf("Error lookup: %d\n", i);
	  error = 1;
	}
  }
  if (sk->insert(998) > 0) {
	printf("Error insert: %d\n", 998);
	error = 1;
  }
  if (sk->lookup(2000) > 0) {
	  printf("Error lookup: %d\n", 2000);
	  error = 1;
  }
  if (sk->insert(2000) <= 0) {
	printf("Error insert: %d\n", 2000);
	error = 1;
  }
  if (sk->lookup(2000) <= 0) {
	printf("Error lookup: %d\n", 2000);
	error = 1;
  }
  SkipList* sk2 = new SkipList(2, 4);
  for (int i = 0; i < 20; i+=4) {
	if (sk2->insert(i) <= 0)  {
	  printf("Error making small list: %d\n", i);
	  error = 1;
	}
  }
  for (int i = 1; i < 30; i+=5) {
	if (sk2->insert(i) <=0 && i%4 != 0) {
	  printf("Error making small list: %d\n", i);
	  error = 1;
	}
  }
  for (int i = 26; i >= 1; i-=5) {
	if (sk2->lookup(i) != 1) {
	  printf("Error lookup by 5 in small list: %d\n", i);
	  error = 1;	  
	}
  }
  for (int i = 16; i >= 0; i-=4) {
	if (sk2->lookup(i) != 1) {
	  printf("Error lookup by 4 in small list: %d\n", i);
	  error = 1;	  
	}
  }
  if (!error) {
	printf("PASSED\n");
  } else {
	sk2->pretty_print_skiplist();
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
	basic_test();
	return 0;
  }
  srand(time(NULL));
  int LIST_SIZE = atoi(argv[1]);
  int PROBABILITY = atoi(argv[2]);
  int POOL = 3*LIST_SIZE;
  int ITERATIONS = 10000;
  int seconds = 10;
  SkipList* stest2 = SkipList::init_list(LIST_SIZE, POOL, PROBABILITY);
  assert(stest2);
  timespec ts;
  if (LIST_SIZE < 100) {
	stest2->pretty_print_skiplist();
  }
  stest2->enable_counts();
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t start = ts.tv_sec*1000000000 + ts.tv_nsec;
  for (int i = 0; i < ITERATIONS; i++) {
	int key = (rand() % POOL);
	if (ITERATIONS < 50) printf("key: %d\n", key);
	stest2->lookup(key);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t lookup_time = ts.tv_sec*1000000000 + ts.tv_nsec;
  stest2->disable_counts();
  for (int i = LIST_SIZE; i < LIST_SIZE+ITERATIONS; i++) {
	stest2->insert(rand() % POOL);
  }
  clock_gettime(CLOCK_MONOTONIC, &ts);
  time_t insert_time = ts.tv_sec*1000000000 + ts.tv_nsec;

  printf("insert: %ld; lookup: %ld; itr: %d; size: %d; levels: %d; probability: %d; ptr: %d\n", 
		 (insert_time-lookup_time)/(ITERATIONS), (lookup_time-start)/(ITERATIONS), ITERATIONS, LIST_SIZE, int(floor(log(LIST_SIZE)/log(PROBABILITY))), PROBABILITY, stest2->get_ptr_count());
}


// [DONE] Read-only not-concurrent skip list working
// [DONE] Measure perf
// [DONE] Verify O(logn) performance
// [DONE] bench.py to run with different sizes, write output.
// [DONE] verify log_2 performance
// [DONE] re-order node array to move prefixes and next nodes together, benchmark
// order arrays so top level is at 0.
// log_4 algorithm read-only (not concurrent?) skip list working
// Fix amount of time, not amount of work.  setitimer.
// multi-threaded measure perf
// Concurrent skip list working

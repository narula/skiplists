#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <cstdlib>
#include <math.h>

#define MAX_LEVELS 3

struct next {
  int prefix;
  struct node2* nxt;
};

struct next4 {
  int prefix;
  struct node4* nxt;
};

struct node2 {
  int key;
  int topLevel;
  struct next nexts[MAX_LEVELS];
  next* nn(int level) {
	return &(nexts[topLevel - 1 - level]);
  }
};

struct node4 {
  int key;
  int topLevel;
  struct next4 nexts[MAX_LEVELS];
  struct next4 nexts_1[MAX_LEVELS];
  struct next4 nexts_2[MAX_LEVELS];
  next4* nn(int level) {
	return &(nexts[topLevel - 1 - level]);
  }
  next4* nn1(int level) {
	return &(nexts_1[topLevel - 1 - level]);
  }
  next4* nn2(int level) {
	return &(nexts_2[topLevel - 1 - level]);
  }
};

void create_2(int sz) {
  struct node2 sk[sz+2];

  // set up head
  sk[0].key = -1;
  sk[0].topLevel = MAX_LEVELS;
  for (int i = 0; i < MAX_LEVELS; i++) {
	sk[0].nexts[i].prefix = int(pow(2, i))-1;
	sk[0].nexts[i].nxt = &(sk[int(pow(2, i))-1]);
  }

  for (int i = 1; i < sz+1; i++) {
	sk[i].key = i-1;
	sk[i].topLevel = 1;
	for (int k = 2; k < MAX_LEVELS; k++) {
	  if (i % int(pow(2, k-1)) == 0) {
		sk[i].topLevel = k;
	  }	
	}
	// fix up next when near end for +1 level
	int np = sk[i].key+1;
	node2* nptr = &(sk[i+1]);
	if (np >= sz) {
	  np = INT_MAX;
	  nptr = &(sk[sz+1]);
	}
	sk[i].nexts[0].prefix = np;
	sk[i].nexts[0].nxt = nptr;

	for (int j = 1; j < sk[i].topLevel; j++) {
	  int nextp = int(pow(2, j)) + i - 1;
	  node2* nextptr = &(sk[nextp]);
	  if (nextp >= sz) {
		nextp = INT_MAX;
		nextptr = &(sk[sz+1]);
	  }
	  sk[i].nexts[j].prefix = nextp;
	  sk[i].nexts[j].nxt = nextptr;
	}
	int last = sk[i].topLevel-1;
	if (sk[i].nexts[last].prefix >= sz) {
	  sk[i].nexts[last].prefix = INT_MAX;
	  sk[i].nexts[last].nxt = &(sk[sz+1]);
	}
  }

  // set up tail
  sk[sz+1].key = INT_MAX;
  sk[sz+1].topLevel = MAX_LEVELS;

  for (int i = 0; i < sz+2; i++) {
	printf("[K:%02d ", sk[i].key);
	for (int j = 0; j < sk[i].topLevel; j++) {
	  printf("N:%02d ", sk[i].nexts[j].prefix);
	}
	printf("]\n");
  }
}


void create_4(int sz) {
  struct node4 sk[sz+2];

  // set up head
  sk[0].key = -1;
  sk[0].topLevel = MAX_LEVELS;
  for (int i = 0; i < MAX_LEVELS; i++) {
	sk[0].nexts[i].prefix = int(pow(4, i))-1;
	sk[0].nexts[i].nxt = &(sk[int(pow(4, i))-1]);
  }

  for (int i = 1; i < sz+1; i++) {
	sk[i].key = i-1;
	sk[i].topLevel = 1;
	for (int k = 2; k < MAX_LEVELS; k++) {
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
	sk[i].nexts[0].prefix = np;
	sk[i].nexts[0].nxt = nptr;

	for (int j = 1; j < sk[i].topLevel; j++) {
	  int nextp = int(pow(4, j)) + i - 1;
	  node4* nextptr = &(sk[nextp]);
	  if (nextp >= sz) {
		nextp = INT_MAX;
		nextptr = &(sk[sz+1]);
	  }
	  sk[i].nexts[j].prefix = nextp;
	  sk[i].nexts[j].nxt = nextptr;
	}
	int last = sk[i].topLevel-1;
	if (sk[i].nexts[last].prefix >= sz) {
	  sk[i].nexts[last].prefix = INT_MAX;
	  sk[i].nexts[last].nxt = &(sk[sz+1]);
	}
  }

  // set up tail
  sk[sz+1].key = INT_MAX;
  sk[sz+1].topLevel = MAX_LEVELS;

  for (int i = 0; i < sz+2; i++) {
	printf("[K:%02d ", sk[i].key);
	for (int j = 0; j < sk[i].topLevel; j++) {
	  printf("N:%02d ", sk[i].nexts[j].prefix);
	}
	printf("]\n");
  }
}

int main(int argc, char** argv) {
  create_2(16);
  create_4(16);
}

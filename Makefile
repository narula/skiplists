
all: skiplist

skiplist: skiplist.o
	g++ skiplist.o -lrt -o skiplist

skiplist.o: skiplist.cc
	g++ -c -O3 skiplist.cc

clean:
	rm -rf *.o skiplist

debug:
	g++ -DDEBUG -g -c skiplist.cc
	g++ -DDEBUG -g skiplist.o -lrt -o skiplist
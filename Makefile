
all: skiplist test2 skiplist2 skiplist-f

skiplist: skiplist.o
	g++ skiplist.o -lrt -o skiplist

skiplist.o: skiplist.cc
	g++ -c -O3 skiplist.cc

clean:
	rm -rf *.o skiplist test2 skiplist2 skiplist-f

debug:
	g++ -DDEBUG -g -c skiplist.cc
	g++ -DDEBUG -g skiplist.o -lrt -o skiplist
	g++ -DDEBUG -g -c -O3 test2.cc
	g++ -DDEBUG -g test2.o -lrt -o test2
	g++ -DDEBUG -g -c skiplist2.cc
	g++ -DDEBUG -g skiplist2.o -lrt -o skiplist2
	g++ -DDEBUG -g -c skiplist-fanout.cc
	g++ -DDEBUG -g skiplist-fanout.o -lrt -o skiplist-f

test2: 
	g++ -c -O3 test2.cc
	g++ test2.o -lrt -o test2

skiplist2:
	g++ -c -O3 skiplist2.cc
	g++ skiplist2.o -lrt -o skiplist2

skiplist-f:
	g++ -c -O3 skiplist-fanout.cc
	g++ skiplist-fanout.o -lrt -o skiplist-f

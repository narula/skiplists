
all: skiplist skiplist2 skiplist-f skiplist-e

skiplist: skiplist.o
	g++ skiplist.o -lrt -o skiplist

skiplist.o: skiplist.cc
	g++ -c -O3 skiplist.cc

clean:
	rm -rf *.o skiplist skiplist2 skiplist-f skiplist-e

debug:
	g++ -DDEBUG -g -c skiplist.cc
	g++ -DDEBUG -g skiplist.o -lrt -o skiplist
	g++ -DDEBUG -g -c skiplist2.cc
	g++ -DDEBUG -g skiplist2.o -lrt -o skiplist2
	g++ -DDEBUG -g -c skiplist-fanout.cc
	g++ -DDEBUG -g skiplist-fanout.o -lrt -o skiplist-f
	g++ -DDEBUG -g -c skiplist-extra.cc
	g++ -DDEBUG -g skiplist-extra.o -lrt -o skiplist-e

skiplist2:
	g++ -c -O3 skiplist2.cc
	g++ skiplist2.o -lrt -o skiplist2

skiplist-f:
	g++ -c -O3 skiplist-fanout.cc
	g++ skiplist-fanout.o -lrt -o skiplist-f

skiplist-e:
	g++ -c -O3 skiplist-extra.cc
	g++ skiplist-extra.o -lrt -o skiplist-e
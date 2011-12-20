
all: skiplist test2 skiplist4 skiplist2

skiplist: skiplist.o
	g++ skiplist.o -lrt -o skiplist

skiplist.o: skiplist.cc
	g++ -c -O3 skiplist.cc

clean:
	rm -rf *.o skiplist test2 skiplist4 skiplist2

debug:
	g++ -DDEBUG -g -c skiplist.cc
	g++ -DDEBUG -g skiplist.o -lrt -o skiplist
	g++ -DDEBUG -g -c -O3 test2.cc
	g++ -DDEBUG -g test2.o -lrt -o test2
	g++ -DDEBUG -g -c skiplist4.cc
	g++ -DDEBUG -g skiplist4.o -lrt -o skiplist4
	g++ -DDEBUG -g -c skiplist2.cc
	g++ -DDEBUG -g skiplist2.o -lrt -o skiplist2

test2: 
	g++ -c -O3 test2.cc
	g++ test2.o -lrt -o test2

skiplist4:
	g++ -c -O3 skiplist4.cc
	g++ skiplist4.o -lrt -o skiplist4

skiplist2:
	g++ -c -O3 skiplist2.cc
	g++ skiplist2.o -lrt -o skiplist2

all: skiplist2 skiplist4

clean:
	rm -rf *.o skiplist4 skiplist2

debug:
	g++ -DDEBUG -g -c skiplist2.cc
	g++ -DDEBUG -g skiplist2.o -lrt -o skiplist2
	g++ -DDEBUG -g -c skiplist4.cc
	g++ -DDEBUG -g skiplist4.o -lrt -o skiplist4

skiplist2:
	g++ -c -O3 skiplist2.cc
	g++ skiplist2.o -lrt -o skiplist2

skiplist4:
	g++ -c -O3 skiplist4.cc
	g++ skiplist4.o -lrt -o skiplist4

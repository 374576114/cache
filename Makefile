test : main.cpp benchmark.h lib/arc.h lib/lirs.h lib/lru.h lib/mq.h trace/reader.h
	g++ -o test -ggdb main.cpp

clean:
	rm test

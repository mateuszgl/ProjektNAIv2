all: path



path: path.o helpers.o
	g++ -std=c++0x  -fopenmp  path.o helpers.o -lSDL -o path
path.o: path.cpp
	g++ -std=c++0x -c -fopenmp -O2 path.cpp -lSDL
helpers.o: helpers.cpp
	g++ -std=c++0x -c -fopenmp  -O2 helpers.cpp -lSDL

dist: fuzzy.tar.gz

fuzzy.tar.gz: path
	tar -czf przykladagwiazdka.tar.gz  path.cpp pathnoanim.cpp Makefile map.txt

clean:
	rm -f path *.o
	rm -f fuzzy.tar.gz


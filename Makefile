run:   #default
	g++ -o main src/main.cpp -fopenmp
bench: # called with make bench
	g++ -o main src/main.cpp -fopenmp -DBENCH -Iinclude -Llib -lbenchmark -O3 -ffast-math -lshlwapi

objects = ops.o path.o dir.o cow-ops.o cow-path.o cow-dir.o
include = -I../include

%.o: %.cc
	i686-w64-mingw32-g++ -std=c++17 -O3 -march=native -g $(include) -c $< -o $@

libfs.a: $(objects)
	i686-w64-mingw32-ar rc $@ $^

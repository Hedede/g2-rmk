objects = fsops.o fspath.o
include = -I../include -I/home/shared/devel/Projects/awlib/include

%.o: %.c++
	i686-w64-mingw32-g++ -std=c++17 $(include) -c $< -o $@

libfs.a: $(objects)
	i686-w64-mingw32-ar rc $@ $^

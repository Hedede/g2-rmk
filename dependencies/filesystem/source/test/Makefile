objects = test.o
include = -I../../include/aw/fallback/std -I/home/shared/devel/Projects/awlib/include

%.o: %.c++
	i686-w64-mingw32-g++ -std=c++17 $(include) -c $< -o $@

test.exe: $(objects)
	i686-w64-mingw32-g++ -static-libgcc -static-libstdc++ -static -lpthread -o $@ $< ../libfs.a

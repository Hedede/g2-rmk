sources=\
Log.cpp \
Main.cpp \
Win32.cpp

objects=$(patsubst %.cpp, %.o, $(sources))

awlib_path=/home/awdev/awlib

includes=\
-I../../libGothic/include \
-I../../libRemake/include \
-I$(awlib_path)/include

cxx_flags = -std=c++1z -g -fno-strict-aliasing -fno-exceptions -fno-rtti

%.o: %.cpp
	@ echo [Build] Compiling $@
	@ i686-w64-mingw32-g++ $(includes) $(cxx_flags) -c $< -o $@

hook: $(objects)
	i686-w64-mingw32-g++ $(includes) $(cxx_flags) -static-libgcc -static-libstdc++ -static -lwinpthread -shared $(objects) -lgdi32 -lws2_32 -lcomctl32 -o testlib.dll

rebuild:
	touch $(sources)

include ../../config.mk

sources=\
Log.cpp \
Main.cpp \
Win32.cpp \
Clobber.cpp \
Game/CGameManager.cpp \
Menu/zMenu.cpp

objects=$(patsubst %.cpp, %.o, $(sources))

libraries=\
-L$(sdl_path)/lib
includes=\
-I../../libGothic/include \
-I../../libRemake/include \
-I$(awlib_path)/include \
-I$(sdl_path)/include

cxx_flags = -std=c++1z -g -fno-strict-aliasing -fno-exceptions -fno-rtti

linker_flags=$(libraries)
static_gcc=-static-libgcc -static-libstdc++ -static -lwinpthread
libs=-shared -lgdi32 -lws2_32 -lSDL2_image -lSDL2

%.o: %.cpp
	@ echo [Build] Compiling $@
	@ i686-w64-mingw32-g++ $(includes) $(cxx_flags) -c $< -o $@

hook: $(objects)
	i686-w64-mingw32-g++ $(includes) $(cxx_flags) $(linker_flags) $(static_gcc) $(objects) $(libs) -o testlib.dll

rebuild:
	touch $(sources)

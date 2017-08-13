include ../../config.mk

sources=\
Log.cpp \
Main.cpp \
Win32.cpp \
Clobber.cpp \
Game/CGameManager.cpp \
Graphics/SplashWindow.cpp \
Graphics/FontMan.cpp \
Menu/zMenu.cpp \
Input/InputCallback.cpp

objects=$(patsubst %.cpp, %.o, $(sources))

libraries=\
-Wl,-rpath,$(sdl_path)/lib\
-L$(sdl_path)/lib
includes=\
-I../../libGothic/include \
-I../../libRemake/include \
-I$(awlib_path)/include \
-I$(sdl_path)/include

#CXX = winegcc -m32
CXX = i686-w64-mingw32-g++

cxx_flags = -std=c++1z -g -fno-strict-aliasing -fno-exceptions -fno-rtti

linker_flags=$(libraries)
static_gcc=-static-libgcc -static-libstdc++ -static -lwinpthread
libs= -lgdi32 -lSDL2_image -lSDL2main -lSDL2

%.o: %.cpp
	@ echo [Build] Compiling $@
	@ $(CXX) $(includes) $(cxx_flags) -c $< -o $@

hook: $(objects)
	@ echo [Build] Linking:
	$(CXX) $(includes) $(cxx_flags) $(static_gcc) $(objects) $(linker_flags) $(libs) -shared -o testlib.dll

rebuild:
	touch $(sources)

linecount:
	wc -l $(sources)

include ../config.mk

sources=\
src/Log.cpp \
src/Main.cpp \
src/Win32.cpp \
src/Clobber.cpp \
src/Game/CGameManager.cpp \
src/Game/GameSession.cpp \
src/Game/World.cpp \
src/Graphics/SplashWindow.cpp \
src/Graphics/FontMan.cpp \
src/Graphics/Camera.cpp \
src/Sound/zCSndSys_OpenAL.cpp \
src/Sound/zCSndFX_OpenAL.cpp \
src/Sound/SoundOpenAL.cpp \
src/Sound/SourcePool.cpp \
src/System/zResource.cpp \
src/Filesystem/filesystem.cpp \
src/Filesystem/find_files.cpp \
src/Menu/zMenu.cpp \
src/Input/InputCallback.cpp

objects=$(patsubst src/%.cpp, build/%.o, $(sources))

libraries=
includes=\
-I./src \
-I$(awlib_path)/include \
-I../dependencies/filesystem/include

lib_path=../dependencies/lib
#USE_WINE=true
ifeq ($(USE_WINE), true)
CXX = winegcc -m32
else
libraries += -Wl,-rpath,$(sdl_path)/lib -L$(sdl_path)/lib
libraries += -L/usr/i686-w64-mingw32/lib -lOpenAL32.dll
includes  += -I$(sdl_path)/include
CXX = i686-w64-mingw32-g++
endif


cxx_flags = -std=c++1z -g -fno-strict-aliasing -fexceptions -fno-rtti
pp_flags  = -DNOMINMAX -DAW_STATIC_BUILD

linker_flags=$(libraries)
ifeq ($(USE_WINE), true)
static_gcc=-lstdc++ -lpthread
else
static_gcc=-static-libgcc -static-libstdc++ -static -lwinpthread
endif
libs= -L$(lib_path) -lgdi32 -lSDL2_image -lSDL2main -lSDL2 -lawaudio -lawio -lfs

build/%.o: src/%.cpp
	@ mkdir -p $(dir $@)
	@ echo [Build] Compiling $@
	@ $(CXX) $(includes) $(cxx_flags) $(pp_flags) -c $< -o $@

hook: $(objects)
	@ echo [Build] Linking:
	$(CXX) $(static_gcc) $(objects) $(linker_flags) $(libs) -shared -o build/testlib.dll
	cp -f build/testlib.dll $(gothic_path)/System

rebuild:
	touch $(sources)

linecount:
	wc -l $(sources)

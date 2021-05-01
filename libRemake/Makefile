include ../config.mk

sources=\
src/Log.cpp \
src/Main.cpp \
src/Win32.cpp \
src/Clobber.cpp \
src/ZenGin/zOption.cpp \
src/ZenGin/zVob.cpp \
src/Hook/statics.cpp \
src/Script/Parser.cpp \
src/Game/CGameManager.cpp \
src/Game/GameSession.cpp \
src/Game/World.cpp \
src/Game/Game.cpp \
src/Game/Externals.cpp \
src/Graphics/SplashWindow.cpp \
src/Graphics/FontMan.cpp \
src/Graphics/Camera.cpp \
src/Camera/zMovementTracker.cpp \
src/Sound/zCSndSys_OpenAL.cpp \
src/Sound/zCSndFX_OpenAL.cpp \
src/Sound/SoundOpenAL.cpp \
src/Sound/SourcePool.cpp \
src/System/zResource.cpp \
src/Menu/zMenu.cpp \
src/Input/InputCallback.cpp \
src/Gothic/Game/oItem.cpp \
src/TypeInfo/oCWorld.cpp \

optimized=\
src/Debug/debugger_tools.cpp \
src/Utility/String.cpp \
src/Filesystem/filesystem.cpp \
src/Filesystem/find_files.cpp \

objects =$(patsubst src/%.cpp, build/%.o, $(sources))

objects+=$(patsubst src/%.cpp, build/opt/%.o, $(optimized))

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


cxx_flags = -std=c++1z -g -fno-rtti
pp_flags  = -DNOMINMAX -DAW_STATIC_BUILD

#optimization
#cxx_flags += -Og -finline-functions -finline-functions-called-once  -fno-omit-frame-pointer
cxx_flags += -fno-strict-aliasing

linker_flags=$(libraries)
ifeq ($(USE_WINE), true)
static_gcc=-lstdc++ -lpthread
else
#static_gcc=-static-libgcc -static-libstdc++ -static -lwinpthread
#static_gcc=-lstdc++ -lpthread
endif
libs= -L$(lib_path) -ldbghelp -lOpenAL32 -lgdi32 -lSDL2_image -lSDL2main -lSDL2 -lawaudio -lawio -lfs

build/opt/%.o: src/%.cpp
	@ mkdir -p $(dir $@)
	@ echo [Build] Compiling $@
	@ $(CXX) $(includes) -masm=intel -fexceptions -Og $(cxx_flags) $(pp_flags) -c $< -o $@

build/%.o: src/%.cpp
	@ mkdir -p $(dir $@)
	@ echo [Build] Compiling $@
	@ $(CXX) $(includes) -masm=intel -fno-exceptions $(cxx_flags) $(pp_flags) -c $< -o $@

hook: $(objects)
	@ echo [Build] Linking:
	$(CXX) $(static_gcc) $(objects) $(linker_flags) $(libs) -shared -o build/testlib.dll
	cp -f build/testlib.dll $(gothic_path)/System

rebuild:
	touch $(sources) $(optimized)

linecount:
	wc -l $(sources)

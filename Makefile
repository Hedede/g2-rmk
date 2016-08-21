includes  = -IlibGothic/include
cxx_flags = -std=c++1z -fno-exceptions -fno-rtti

hook:
	i686-w64-mingw32-g++ $(includes) $(cxx_flags) -static-libgcc -static-libstdc++ -static -lwinpthread -shared testlib.cpp -o testlib.dll

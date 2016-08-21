#ifndef G2_zSTRING_h
#define G2_zSTRING_h
#include <Types/Base.h>

char const spaceChar = ' ';

struct zSTRING {
	zSTRING() = default;

private:
	void* _vtab;
	void* data[4];
};
#endif//G2_zSTRING_h

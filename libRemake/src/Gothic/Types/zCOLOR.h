#pragma once
#include <cstdlib>
struct zCOLOR {
	zCOLOR() = default;
	zCOLOR(zCOLOR const&) = default;
	zCOLOR(char r, char g, char b, char a)
	{
		values[0] = b;
		values[1] = g;
		values[2] = r;
		values[3] = a;
	}

	zCOLOR& operator=(zCOLOR const&) = default;

	char values[4];
};

namespace colors {
static const zCOLOR black{0,0,0,0};
}

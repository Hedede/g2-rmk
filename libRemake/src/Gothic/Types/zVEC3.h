#ifndef Gothic_zVEC3_H
#define Gothic_zVEC3_H
#include <cmath>

struct zVEC3 {
	float Length()
	{
		return std::sqrt(x*x+y*y+z*z);
	}
	float x,y,z;
};

inline zVEC3 operator-(zVEC3 const& a, zVEC3 const& b)
{
	return {a.x-b.x,a.y-b.y,a.z-b.z};
}

#endif//Gothic_zVEC3_H

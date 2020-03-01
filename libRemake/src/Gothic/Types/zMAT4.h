#pragma once
#include <array>
#include <Gothic/Types/zVEC3.h>
#include <Hook/Externals.h>



struct zMAT4 {
	zVEC3 GetTranslation() const
	{
		//zVEC3 ret;
		//Thiscall<void(zMAT4 const*, zVEC3&)> call{0x408EE0};
		//call(this,ret);
		//return ret;
		return {m[0][3], m[1][3], m[2][3]};
	}

	void SetTranslation(zVEC3 const& vec)
	{
		m[0][3] = vec.x;
		m[1][3] = vec.y;
		m[2][3] = vec.z;
	}

	zVEC3 GetAtVector() const
	{
		return {m[0][2], m[1][2], m[2][2]};
	}

	zVEC3 GetUpVector() const
	{
		return {m[0][1], m[1][1], m[2][1]};
	}

	zVEC3 GetRightVector() const
	{
		return {m[0][0], m[1][0], m[2][0]};
	}

	float m[4][4];
};

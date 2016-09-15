#pragma once
#include <Gothic/Types/Base.h>
#include <Hook/Externals.h>

struct zCMemPoolBase {
protected:
	static zBOOL& GetDisablePools()
	{
		static zBOOL& s_disablePools = Value<zBOOL>(0x8A2558);
		return s_disablePools;
	}
public:
	static void DisablePools(int b)
	{
		GetDisablePools() = b;
	}
};

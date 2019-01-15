#pragma once
#include <Hook/Externals.h>
struct zCRayTurboAdmin {
private:
	static int& PolyThreshold() 
	{
		static int& s_iPolyTreshold = Value<int>(0x89FE9C);
		return s_iPolyTreshold;
	}

public:
	static int GetPolyTreshold()
	{
		return PolyThreshold();
	}

	static void SetPolyTreshold(int v)
	{
		PolyThreshold() = v;
	}
};

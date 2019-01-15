#pragma once
struct zCViewBase;
struct zCLineCache {
	void Flush(zCViewBase* view)
	{
		Thiscall<void(zCLineCache*, zCViewBase*)> call{0x50B3B0};
		call(this, view);
	}
};

auto& zlineCache = Value<zCLineCache>(0x8D42F8);

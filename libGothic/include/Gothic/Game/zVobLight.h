#pragma once
struct zCVobLight {
	static void LoadLightPresets()
	{
		Cdecl<void()> func{0x608690};
		func();
	}
};

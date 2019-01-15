#pragma once
struct zCParticleFX {
	static void InitParticleFX(void)
	{
		Cdecl<void()> call{0x5ACB50};
		call();
	}
};

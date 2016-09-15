#pragma once
struct zCLensFlareFX {
	static void LoadLensFlareScript()
	{
		Cdecl<void()> call{0x562650};
		call();
	}
};

#pragma once

struct zCRenderManager {
	void InitRenderManager()
	{
		Thiscall<void(zCRenderManager*)> call{0x5D74D0};
		call(this);
	}
};

static auto& zrenderMan = Value<zCRenderManager>(0x99AA44);

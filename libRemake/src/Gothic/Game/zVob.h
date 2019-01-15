#pragma once
#include <Gothic/Types/zTree.h>
#include <Gothic/Types/zMAT4.h>
#include <Gothic/Types/zBBox.h>
struct zCWorld;
struct zCVob : zCObject {
	static void InitVobSystem()
	{
		Cdecl<void()> func{0x5FE6E0};
		func();
	}

	void DoFrameActivity()
	{
		Thiscall<void(zCVob*)> call{0x602C60};
		call(this);
	}
		
	zVEC3 GetPositionWorld() const
	{
		return trafoObjToWorld.GetTranslation();
	}

	zVEC3 GetAtVectorWorld() const
	{
		return trafoObjToWorld.GetAtVector();
	}

	zVEC3 GetUpVectorWorld() const
	{
		return trafoObjToWorld.GetUpVector();
	}

	char data[0x18];
	zMAT4 trafoObjToWorld;
	zTBBox3D bbox3D;
	char data1[0x24];
	zCWorld *homeWorld;
	char data2[0x64];
};
CHECK_SIZE(zCVob, 0x120);

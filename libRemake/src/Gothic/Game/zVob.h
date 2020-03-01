#pragma once
#include <Gothic/Types/zTree.h>
#include <Gothic/Types/zMAT4.h>
#include <Gothic/Types/zBBox.h>
struct zCWorld;
struct zCRigidBody;
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

	void SetPositionWorld(zVEC3 const& position)
	{
		Thiscall<void(zCVob*,zVEC3 const&)> call(0x61BB70);
		call(this,position);
		//trafoObjToWorld.SetTranslation(position);
	}

	zVEC3 GetAtVectorWorld() const
	{
		return trafoObjToWorld.GetAtVector();
	}

	zVEC3 GetUpVectorWorld() const
	{
		return trafoObjToWorld.GetUpVector();
	}

	 zCRigidBody* GetRigidBody()
	 {
		 Thiscall<zCRigidBody*(zCVob*)> call(0x5FE960);
		 return call(this);
	 }

	char data[0x18];
	zMAT4 trafoObjToWorld;
	zTBBox3D bbox3D;
	char data1[0x24];
	zCWorld *homeWorld;
	char data2[0x64];
};
CHECK_SIZE(zCVob, 0x120);

struct oCVob : zCVob {

};

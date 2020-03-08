#ifndef ZenGin_Vob_H
#define ZenGin_Vob_H
#include <Gothic/Types/zTree.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zMAT4.h>
#include <Gothic/Types/zBBox.h>
#include <Gothic/Types/zCObject.h>
#include <Types/string_view.h>

struct zCWorld;
struct zCRigidBody;
struct zCBspLeaf;
struct zTTraceRayReport;
struct zCEventMessage;
struct zCVisual;
struct zCVob;

enum zTVobSleepingMode {
	zVOB_SLEEPING,
	zVOB_AWAKE,
	zVOB_AWAKE_DOAI_ONLY
};

struct zCVob_vt {
	zCClassDef *(__thiscall *GetClassDef)(zCVob *);
	void (__thiscall *Archive)(zCVob *, zCArchiver *);
	void (__thiscall *Unarchive)(zCVob *, zCArchiver *);
	void (__thiscall *dtor)(zCVob *, int);
	void (__thiscall *OnTrigger)(zCVob *, zCVob *, zCVob *);
	void (__thiscall *OnUntrigger)(zCVob *, zCVob *, zCVob *);
	void (__thiscall *OnTouch)(zCVob *, zCVob *);
	void (__thiscall *OnUntouch)(zCVob *, zCVob *);
	void (__thiscall *OnTouchLevel)(zCVob *);
	void (__thiscall *OnDamage)(zCVob *, zCVob *, zCVob *, float, int, const zVEC3 *);
	void (__thiscall *OnMessage)(zCVob *, zCEventMessage *, zCVob *);
	void (__thiscall *OnTick)(zCVob *);
	void (__thiscall *OnTimer)(zCVob *);
	void (__thiscall *PostLoad)(zCVob *);
	int (__thiscall *GetCharacterClass)(zCVob *);
	void (__thiscall *SetSleepingMode)(zCVob *, zTVobSleepingMode);
	void (__thiscall *EndMovement)(zCVob *, int);
	void (__thiscall *CanThisCollideWith)(zCVob *, zCVob *);
	void (__thiscall *Render)(zCVob *, int *);
	void (__thiscall *SetVisual1)(zCVob *, const zSTRING&);
	void (__thiscall *SetVisual2)(zCVob *, zCVisual *);
	void (__thiscall *GetScriptInstance)(zCVob *, zSTRING **, int *);
	void (__thiscall *SetByScriptInstance)(zCVob *, const zSTRING *, int);
	void (__thiscall *GetCSStateFlags)(zCVob *);
	int (__thiscall *TraceRay)(zCVob *, const zVEC3 *, const zVEC3 *, int, zTTraceRayReport *);
	void (__thiscall *GetTriggerTarget)(zCVob *, int);
	void (__thiscall *ThisVobAddedToWorld)(zCVob *, zCWorld *);
	void (__thiscall *ThisVobRemovedFromWorld)(zCVob *, zCWorld *);
};


struct zCVob : zCObject {
	zCVob()
	{
		thiscall(0x5FE1E0, this);
	}

	static void InitVobSystem()
	{
		call(0x5FE6E0);
	}

	void DoFrameActivity()
	{
		thiscall(0x602C60, this);
	}
		
	zVEC3 GetPositionWorld() const
	{
		return trafoObjToWorld.GetTranslation();
	}

	void SetPositionWorld(zVEC3 const& position)
	{
		thiscall(0x61BB70, this, &position);
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
		return thiscall<zCRigidBody*>(0x5FE960, this);
	}

	void SetVobName(std::string_view name)
	{
		zSTRING tmpstr = name;
		thiscall(0x5FFDD0, this, &tmpstr);
	}

	zCTree<zCVob>* globalVobTreeNode;

	int      lastTimeDrawn;
	unsigned lastTimeCollected;
	zCArray<zCBspLeaf*> vobLeafList;

	zMAT4 trafoObjToWorld;
	zTBBox3D bbox3D;
	char data1[0x24];
	zCWorld *homeWorld;
	char data2[0x64];
};

struct zCVobLevelCompo : zCVob {
	zCLASS_DECLARATION(zCVobLevelCompo);
public:
	void SetVisual(std::string_view visualname)
	{
		reinterpret_cast<zCVob_vt*>(_vtab)->SetVisual1(this, visualname);
	}
};

struct oCVob : zCVob {

};
#endif//ZenGin_Vob_H

#ifndef Gothic_zSession_H
#define Gothic_zSession_H
#include <Gothic/Input/zCInputCallback.h>
#include <Gothic/Types/zSTRING.h>

struct zCCSManager;
struct zCWorld;
struct oCWorld;
struct zCCamera;
struct zCAICamera;
struct zCVob;
struct oCNpc;
struct zCView;

struct zCSession;
struct oCGame;
struct oCGameInfo;

struct zCSession_vt : zCInputCallback_vt {
	// zCSession:
	void (__thiscall *dtor)(zCSession *);
	void (__thiscall *Init)(zCSession *);
	void (__thiscall *Done)(zCSession *);
	void (__thiscall *Render)(zCSession *);
	void (__thiscall *RenderBlit)(zCSession *);
	void (__thiscall *CamInit1)(zCSession *, zCVob *, zCCamera *);
	void (__thiscall *CamInit2)(zCSession *);
	void (__thiscall *SetCamera)(zCSession *, zCCamera *);
	zCCamera *(__thiscall *GetCamera)(zCSession *);
	zCAICamera *(__thiscall *GetCameraAI)(zCSession *);
	zCVob *(__thiscall *GetCameraVob)(zCSession *);
	zCView *(__thiscall *GetViewport)(zCSession *);
	zCWorld *(__thiscall *GetWorld)(zCSession *);
	void (__thiscall *SaveWorld)(zCSession *, const zSTRING *, int, int, int);
	void (__thiscall *LoadWorld)(zCSession *, const zSTRING *, int);
	zCCSManager *(__thiscall *GetCutsceneManager)(zCSession *);
	void (__thiscall *SetTime)(zCSession *, int, int, int);
	void (__thiscall *GetTime)(zCSession *, int *, int *, int *);
	void (__thiscall *SetWorld)(zCSession *, zCWorld *);
	void (__thiscall *DesktopInit)(zCSession *);
	void (__thiscall *CutsceneSystemInit)(zCSession *);

	// oCGame:
	void (__thiscall *EnterWorld)(oCGame *, oCNpc *, int, const zSTRING *);
	void (__thiscall *Pause)(oCGame *, int);
	void (__thiscall *Unpause)(oCGame *);
	void (__thiscall *SetDrawWaynet)(oCGame *, int);
	void (__thiscall *GetDrawWaynet)(oCGame *);
	void (__thiscall *RenderWaynet)(oCGame *);
	void (__thiscall *EnvironmentInit)(oCGame *);
	void (__thiscall *SetRanges)(oCGame *);
	void (__thiscall *SetRangesByCommandLine)(oCGame *);
	void (__thiscall *GetStartPos)(oCGame *);
	void (__thiscall *SetGameInfo)(oCGame *, oCGameInfo *);
	void (__thiscall *LoadParserFile)(oCGame *, const zSTRING *);
	void (__thiscall *TriggerChangeLevel)(oCGame *, const zSTRING *, const zSTRING *);
	oCWorld *(__thiscall *GetGameWorld)(oCGame *);
	void (__thiscall *GetGameInfo)(oCGame *);
	zCView *(__thiscall *GetTextView)(oCGame *);
	void (__thiscall *OpenLoadscreen)(oCGame *, bool, zSTRING);
	void (__thiscall *OpenSavescreen)(oCGame *, bool);
	void (__thiscall *CloseLoadscreen)(oCGame *);
	void (__thiscall *CloseSavescreen)(oCGame *);
	void (__thiscall *ChangeLevel)(oCGame *, const zSTRING *, const zSTRING *);
	void (__thiscall *LoadWorldStartup)(oCGame *, const zSTRING *);
	void (__thiscall *LoadWorldStat)(oCGame *, zSTRING);
	void (__thiscall *LoadWorldDyn)(oCGame *, const zSTRING *);
	void (__thiscall *InitWorldSavegame)(oCGame *, int *, zSTRING *);
	int (__thiscall *CheckIfSavegameExists)(oCGame *, const zSTRING *);
	void (__thiscall *CompileWorld)(oCGame *);
	void (__thiscall *WorldInit)(oCGame *);
	void (__thiscall *NpcInit1)(oCGame *, void *); // zCTree<zCVob*>
	void (__thiscall *NpcInit2)(oCGame *);
	void (__thiscall *SetAsPlayer)(oCGame *, const zSTRING *);
};


struct zCSession : zCInputCallback {
	zCSession() : zCInputCallback()
	{
		_vtab = reinterpret_cast<void*>(0x832C4C);
	}

	zCCamera* GetCamera()
	{
		return camera;
	}

	void Render()
	{
		reinterpret_cast<zCSession_vt*>(_vtab)->Render(this);
	}

	void RenderBlit()
	{
		reinterpret_cast<zCSession_vt*>(_vtab)->RenderBlit(this);
	}

	zCCSManager*  csMan    = 0;
	zCWorld*      world    = 0;
	zCCamera*     camera   = 0;
	zCAICamera*   aiCam    = 0;
	zCVob*        camVob   = 0;
	zCView*       viewport = 0;
};

struct oCGame  : zCSession {
	oCGame() : zCSession()
	{
		Thiscall<void(oCGame*)> ctor{0x6BF810};
		ctor(this);
	}
	char data[0x170];
};

// 0x18C

#endif//Gothic_zSession_H

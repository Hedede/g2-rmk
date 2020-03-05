#ifndef GOTHIC_ZMENU_H
#define GOTHIC_ZMENU_H
#include <Hook/Externals.h>
#include <Gothic/Types/zSTRING.h>

struct zCParser;

struct zCMenu;
struct oCMenuSavegame;
struct zCMenuItem;
struct zCMenu_vt {
	void (__thiscall *HandleEvent)(zCMenu*, int);
	void (__thiscall *HandleEnterItem)(zCMenu *, zCMenuItem *);
	void (__thiscall *HandleLeaveItem)(zCMenu *, zCMenuItem *);
	void (__thiscall *HandleActivateItem)(zCMenu *, zCMenuItem *);
	void (__thiscall *Run)(zCMenu *);
	void (__thiscall *Enter)(zCMenu *);
	void (__thiscall *Leave)(zCMenu *);
	void (__thiscall *Render)(zCMenu *);
	void (__thiscall *RedrawItems)(zCMenu *);
	void (__thiscall *HandleFrame)(zCMenu *, int);
	void (__thiscall *Activate)(zCMenu *);
	void (__thiscall *dtor)(zCMenu*, size_t);
	void (__thiscall *ScreenInit)(zCMenu *);
	void (__thiscall *ScreenDone)(zCMenu *);
	void (__thiscall *HandleAction)(zCMenu *);
	void (__thiscall *HandleSelAction)(zCMenu *, int, const zSTRING&, zCMenuItem *);
	void (__thiscall *SetByScript)(zCMenu *, const zSTRING&);
	void (__thiscall *ExecCommand)(zCMenu *, const zSTRING&);

	// Menu_savegame vtable
	void (__thiscall *HandleSlotChange)(oCMenuSavegame *, int);
	int  (__thiscall *GetSelectedSlot)(oCMenuSavegame *);
};

struct zCMenu /*::zCInputCallback*/ {
	static zCParser*& menuParser;
	static void CreateParser();
	static zCMenu* Create(std::string const& name)
	{
		Cdecl<zCMenu*(zSTRING const&)> call{0x4DABE0};
		return call(name);
	}

	static void Startup()
	{
		Cdecl<void()> call{0x4D9F90};
		call();
	}

	using Callback = int(*)();
	static Callback& EnterCallback;
	static Callback& LeaveCallback;

	static int& inGameMenu;
	
	zCMenu_vt& vtab()
	{
		return *reinterpret_cast<zCMenu_vt*>(_vtab);
	}

	zCMenu(std::string const& name)
	{
		Thiscall<void(zCMenu*, zSTRING const&)> ctor{0x4DA610};
		ctor(this, name);
	}

	~zCMenu()
	{
		vtab().dtor(this, 1);
	}

	void Release()
	{
		Thiscall<void(zCMenu*)> call{0x4DAB80};
		call(this);
	}

	void Run()
	{
		Thiscall<void(zCMenu*)> call{0x4DB9A0};
		call(this);
	}

protected:
	zCMenu()
	{
		Thiscall<void(zCMenu*)> ctor{0x4DA470};
		ctor(this);
	}

protected:
	void* _vtab;
	char data[0xCC0];
};

#endif//GOTHIC_ZMENU_H

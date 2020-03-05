#ifndef Gothic_oMenuSavegame_H
#define Gothic_oMenuSavegame_H
#include <Gothic/Menu/oMenuMain.h>

struct oCSavegameManager;
struct zCTexture;
struct zCMenuItem;
struct zCView;

struct oCMenuSavegame : oCMenuMain {
	enum class Mode : unsigned {
		Save,
		Load
	};

	oCMenuSavegame(std::string const& name, Mode mode)
		: oCMenuMain(name), mode(mode)
	{
		_vtab = (void*)0x82F1EC;
	}

	int GetSelectedSlot() const
	{
		return selectedSlotId;
	}

	Mode mode;
	oCSavegameManager* saveMan = nullptr;
	int selectedSlotId = -1;
	int selItem        = -1;
	zCTexture *thumbPic = nullptr;
	zCMenuItem *itemPlayTime  = nullptr;
	zCMenuItem *itemDateTime  = nullptr;
	zCMenuItem *itemLevelName = nullptr;
	zCMenuItem *itemGameTime  = nullptr;
	zCMenuItem *itemThumbPic  = nullptr;
	zCView *view = nullptr;
};

#endif//Gothic_oMenuSavegame_H

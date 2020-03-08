#include <Gothic/Game/zOptions.h>

#define zCOption zCOptions

zCOption::zCOption()
{
	thiscall(0x460350, this);
}

void zCOption::Init(std::string_view cmdLine)
{
	zSTRING tmpstr = cmdLine;
	checked::thiscall<void(zCOption*, zSTRING, int)>(0x463C20, this, tmpstr, 1);
}

void zCOption::ChangeDir(zTOptionPath dirId)
{
	thiscall(0x465160, this, dirId);
}

bool zCOption::Load(std::string_view filename)
{
	zSTRING tmpstr = filename;
	return checked::thiscall<int(zCOption*, zSTRING)>(0x4607B0, this, tmpstr);
}

bool zCOption::Save(std::string_view filename)
{
	zSTRING tmpstr = filename;
	return checked::thiscall<int(zCOption*, zSTRING)>(0x4616C0, this, tmpstr);
}

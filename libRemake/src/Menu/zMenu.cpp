#include <Gothic/Menu/zMenu.h>
#include <Gothic/Script/zParser.h>
#include <Gothic/Game/zOptions.h>

zCParser*& zCMenu::menuParser = Value<zCParser*>(0x8D1E68);
void zCMenu::CreateParser()
{
	zoptions->ChangeDir(DIR_SCRIPTS);

	menuParser = new zCParser(100);
}


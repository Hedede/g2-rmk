#include <Gothic/Menu/zMenu.h>
#include <Gothic/Script/zParser.h>
#include <Gothic/Game/zOptions.h>

zCParser*& zCMenu::menuParser = Value<zCParser*>(0x8D1E68);
zCMenu::Callback& zCMenu::EnterCallback = Value<zCMenu::Callback>(0x8D1E7C);
zCMenu::Callback& zCMenu::LeaveCallback = Value<zCMenu::Callback>(0x8D1E80);
int& zCMenu::inGameMenu = Value<int>(0x8D1E78);
void zCMenu::CreateParser()
{
	zoptions->ChangeDir(DIR_SCRIPTS);

	menuParser = new zCParser(100);
}


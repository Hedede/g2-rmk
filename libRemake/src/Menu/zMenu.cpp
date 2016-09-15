#include <Gothic/Menu/zMenu.h>
#include <Gothic/Script/zParser.h>
#include <Gothic/Game/zOptions.h>

zBOOL& zCParser::enableParsing = Value<zBOOL>(0xAB6284);
zCParser*& zCMenu::menuParser = Value<zCParser*>(0x8D1E68);
zCMenu::Callback& zCMenu::EnterCallback = Value<zCMenu::Callback>(0x8D1E7C);
zCMenu::Callback& zCMenu::LeaveCallback = Value<zCMenu::Callback>(0x8D1E80);
int& zCMenu::inGameMenu = Value<int>(0x8D1E78);
void zCMenu::CreateParser()
{
	zoptions->ChangeDir(DIR_SCRIPTS);

	menuParser = new zCParser(100);
}


#include <Gothic/Graphics/zTexture.h>
#include <Gothic/Graphics/zModelPrototype.h>
#include <Gothic/Graphics/zMesh.h>
#include <Gothic/Graphics/zMorphMeshProto.h>
zBOOL& zCTexture::s_globalLoadTextures = Value<zBOOL>(0x8A6AA9);
zBOOL& zCModelPrototype::autoConvertAnis = Value<zBOOL>(0x8A31F8);
zBOOL& zCMesh::autoConvertMeshes = Value<zBOOL>(0x8D8868);
zBOOL& zCMorphMeshProto::autoConvertBinaryFile = Value<zBOOL>(0x8D8CC8);


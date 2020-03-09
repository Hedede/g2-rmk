#include "ZenGin/_dieter/zArchive2container.h"
#include "ZenGin/_dieter/zArchiver.h"
#include "ZenGin/_dieter/zArchiver2.h"
#include "ZenGin/_dieter/zContainer.h"

zCObject* zCArchiverBinSafe::CreateObject(zSTRING const& objName)
{
	zCClassDef* classdef = GetClassDefByString(objname);
	if (!classdef)
		return nullptr;

	zCObject* newinst = classdef->CreateNewInstance();
	if ( !newinst )
		zFAULT("D: zArchiver(zCArchiverBinSafe::CreateObject): could not create object for: " + objName); // 1462
	return newinst;
}

#pragma once
#include <Hook/type_info.h>
#include <Gothic/Game/zWorld.h>

extern RTTICompleteObjectLocator oCWorld_rtti;

struct oCWorld_TypeInfo {
	oCWorld_TypeInfo();

	RTTICompleteObjectLocator* rtti = &oCWorld_rtti;
	zCWorld_vt vt;
};

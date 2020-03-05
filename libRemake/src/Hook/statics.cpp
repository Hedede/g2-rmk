#include <Gothic/Game/oRtnManager.h>
#include <Hook/size_checker.h>
#include <Hook/Externals.h>

CHECK_SIZE(oCRtnManager,0x88);

decltype(rtnMan) rtnMan = Value<oCRtnManager>(0xAB31C8);

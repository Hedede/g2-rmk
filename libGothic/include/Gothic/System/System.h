#ifndef GOTHIC2_SYSTEM_H
#define GOTHIC2_SYSTEM_H

#include <Hook/Externals.h>

namespace {
auto libExit = func<void(*)()>(0x502AB0);
auto sysHardExit = func<void(*)(char const*)>(0x504A80);
auto sysEvent = func<void(*)()>(0x5053E0);
auto sysParseCommandLine = func<void(*)(char const*)>(0x505A50);

auto& PreciseTimer = Value<int>(0x8D3FDC);
auto& pTimeDiv    = Value<int>(0x8D3B94);
auto& pTimeDiv70  = Value<int>(0x8D41C0);
auto& BeginTime   = Value<int>(0x8D4178);
auto& BeginTime70 = Value<int>(0x8D3ED4);
}

static char const APP_NAME[] = "Gothic II — REMAKE — [ver.0] ывфыав вфыв";

#endif//GOTHIC2_SYSTEM_H

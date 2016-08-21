#ifndef GOTHIC2_SYSTEM_H
#define GOTHIC2_SYSTEM_H

#include <Hook/func.h>
#include <Hook/value.h>

static auto libExit = func<void(*)()>(0x502AB0);
static auto sysHardExit = func<void(*)(char const*)>(0x504A80);
static auto sysEvent = func<void(*)()>(0x5053E0);
static auto sysParseCommandLine = func<void(*)(char const*)>(0x505A50);

static auto PreciseTimer = Value<int>(0x8D3FDC);
static auto pTimeDiv    = Value<int>(0x8D3B94);
static auto pTimeDiv70  = Value<int>(0x8D41C0);
static auto BeginTime   = Value<int>(0x8D4178);
static auto BeginTime70 = Value<int>(0x8D3ED4);

static char const APP_NAME[] = "Gothic II — Remake 0";

#endif//GOTHIC2_SYSTEM_H

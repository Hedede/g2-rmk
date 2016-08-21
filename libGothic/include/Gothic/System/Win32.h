#ifndef GOTHIC2_SYSTEM_WIN32_H
#define GOTHIC2_SYSTEM_WIN32_H

#include <cstdint>
#include <Hook/func.h>
#include <Hook/value.h>

static auto sysInstLock        = Value<void*>(0x8D426C);
static auto winDoInstanceCheck = Cdecl<void()>{0x5035C0};
static auto checkWinVersion    = Cdecl<void()>{0x505E90};

static auto hInstApp = Value<void*>(0x8D4220);
static auto hWndApp  = Value<void*>(0x8D422C);
static auto hIconApp = Value<void*>(0x8D4228);
static auto hHandle  = Value<void*>(0x8D4278);
static auto dcScreen = Value<void*>(0x8D4224);

static auto AppWndProc = Value<void*>(0x503770);
static auto mouseInf   = Value<int>(0x8D3BA0);

static auto winMainThreadID = Value<unsigned>(0x8D3D34);
static auto winMainThread   = Value<void*>(0x8D3ED0);

static char(&winModuleName)[260] = Value<char[260]>(0x8D3A90);

static auto winExtraX = Value<int>(0x8D3980);
static auto winExtraY = Value<int>(0x8D3988);
static auto winPosX = Value<int>(0x8D40E8);
static auto winPosY = Value<int>(0x8D40EC);
static auto nWidth  = Value<int>(0x8D40F0);
static auto nHeight = Value<int>(0x8D40F4);
#endif//GOTHIC2_SYSTEM_WIN32_H

#ifndef GOTHIC2_SYSTEM_WIN32_H
#define GOTHIC2_SYSTEM_WIN32_H

#include <cstdint>
#include <Hook/func.h>
#include <Hook/value.h>

namespace {
auto winDoInstanceCheck = func<void(*)()>(0x5035C0);
auto checkWinVersion    = func<void(*)()>(0x505E90);

auto& sysInstLock = Value<void*>(0x8D426C);

auto& hInstApp = Value<void*>(0x8D4220);
auto& hWndApp  = Value<void*>(0x8D422C);
auto& hHandle  = Value<void*>(0x8D4278);
auto& dcScreen = Value<void*>(0x8D4224);

auto& mouseInf   = Value<int>(0x8D3BA0);

auto& winMainThreadID = Value<unsigned>(0x8D3D34);
auto& winMainThread   = Value<void*>(0x8D3ED0);

char(&winModuleName)[260] = Value<char[260]>(0x8D3A90);
}
#endif//GOTHIC2_SYSTEM_WIN32_H

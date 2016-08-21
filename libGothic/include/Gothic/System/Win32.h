#ifndef GOTHIC2_SYSTEM_WIN32_H
#define GOTHIC2_SYSTEM_WIN32_H

#include <Hook/func.h>
#include <Hook/value.h>

auto hInstApp = Value<void*>(0x8D4220);
auto winMainThreadID = Value<unsigned>(0x8D3D34);
auto winMainThread = Value<void*>(0x8D3ED0);

char(&winModuleName)[260] = Value<char[260]>(0x8D3A90);


#endif//GOTHIC2_SYSTEM_WIN32_H

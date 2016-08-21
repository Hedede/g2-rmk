#ifndef GOTHIC2_SYSTEM_H
#define GOTHIC2_SYSTEM_H

#include <Hook/func.h>

auto libExit = func<void(*)()>(0x502AB0);
auto sysEvent = func<void(*)()>(0x5053E0);


#endif//GOTHIC2_SYSTEM_H

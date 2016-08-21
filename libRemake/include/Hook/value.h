#ifndef HOOK_VALUE_H
#define HOOK_VALUE_H
#include <cstdint>

template<typename T>
constexpr T& Value(uintptr_t addr)
{
	return *reinterpret_cast<T*>(addr);
}

#endif//HOOK_VALUE_H

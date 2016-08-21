#ifndef HOOK_FUNC_H
#define HOOK_FUNC_H
#include <cstdint>

template<typename T>
using func = T;

template<typename F>
struct Cdecl;

template<typename R, typename...Args>
struct Cdecl<R(Args...)> {
	uintptr_t addr;
	Cdecl(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		func<R(__cdecl*)(Args...)>(addr)(args...);
	}
};

template<typename F>
struct Stdcall;

template<typename R, typename...Args>
struct Stdcall<R(Args...)> {
	uintptr_t addr;
	Stdcall(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		func<R(__stdcall*)(Args...)>(addr)(args...);
	}

};

template<typename F>
struct Thiscall;

template<typename R, typename...Args>
struct Thiscall<R(Args...)> {
	uintptr_t addr;
	Thiscall(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		(func<R(__thiscall*)(Args...)>(addr))(args...);
	}

};

template<typename F>
struct Fastcall;

template<typename R, typename...Args>
struct Fastcall<R(Args...)> {
	uintptr_t addr;
	Fastcall(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		func<R(__fastcall*)(Args...)>(addr)(args...);
	}

};
#endif//HOOK_FUNC_H

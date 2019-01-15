#ifndef Gothic_Remake_Externals_H
#define Gothic_Remake_Externals_H
#include <cstdint>

#if defined(__WINE__)
#define __thiscall __attribute__((__thiscall__))
#define __stdcall __attribute__((__stdcall__))
#endif

/*!
 * Obtain reference to object at address addr.
 */
template<typename T>
constexpr T& Value(uintptr_t addr)
{
	return *reinterpret_cast<T*>(addr);
}

template<typename T>
constexpr T& Value(void* addr)
{
	return *reinterpret_cast<T*>(addr);
}

template<typename T>
constexpr T const& Value(void const* addr)
{
	return *reinterpret_cast<T const*>(addr);
}


/*!
 * Syntactic sugar for function pointers/references:
 * auto T = func<void(*)()>(addr);
 */
template<typename T>
using func = T;



/*!
 * __cdecl call wrapper
 */
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
		(func<R(__cdecl*)(Args...)>(addr))(args...);
	}
};

/*!
 * __stdcall call wrapper
 */
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
		(func<R(__stdcall*)(Args...)>(addr))(args...);
	}

};

/*!
 * __thiscall call wrapper
 */
template<typename F>
struct Thiscall;

template<typename R, typename...Args>
struct Thiscall<R(Args...)> {
	static_assert(sizeof...(Args) > 0, "Thiscall must have at least one arg.");

	uintptr_t addr;
	Thiscall(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		(func<R(__thiscall*)(Args...)>(addr))(args...);
	}

};

/*!
 * __fastcall call wrapper
 */
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
		(func<R(__fastcall*)(Args...)>(addr))(args...);
	}

};
#endif//Gothic_Remake_Externals_H

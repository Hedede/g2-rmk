#ifndef Gothic_Remake_Externals_H
#define Gothic_Remake_Externals_H
#include <cstdint>

#if defined(__WINE__)
#define __thiscall __attribute__((__thiscall__))
#define __stdcall __attribute__((__stdcall__))
#endif

#define FORCE_INLINE __attribute__((always_inline)) inline

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
		return (func<R(__cdecl*)(Args...)>(addr))(args...);
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
		return (func<R(__stdcall*)(Args...)>(addr))(args...);
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

	using type = R(__thiscall*)(Args...);

	uintptr_t addr;
	Thiscall(uintptr_t addr)
		: addr(addr)
	{ }

	R operator()(Args... args)
	{
		return (func<R(__thiscall*)(Args...)>(addr))(args...);
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
		return (func<R(__fastcall*)(Args...)>(addr))(args...);
	}

};


template<typename T>
struct pass {
	pass(T&& value)
		: value(static_cast<T&&>(value))
	{
	}

	T value;

	operator T() const
	{
		return value;
	}
};

template<typename T>
struct extract_arg_t {
	using type = T;
};

template<typename T>
struct extract_arg_t<pass<T>> {
	using type = T;
};

template<typename T>
using extract_arg = typename extract_arg_t<T>::type;

template<typename R = void, typename...Args>
FORCE_INLINE R thiscall(uintptr_t addr, Args...args)
{
	static_assert(sizeof...(Args) > 0, "Thiscall must have at least one arg.");

	using pointer_type = R(__thiscall*)(extract_arg<Args>...);

	auto func = reinterpret_cast<pointer_type>(addr);
	
	return func(args...);
}

template<typename R = void, typename...Args>
FORCE_INLINE R fastcall(uintptr_t addr, Args...args)
{
	using pointer_type = R(__fastcall*)(extract_arg<Args>...);

	auto func = reinterpret_cast<pointer_type>(addr);
	
	return func(args...);
}

template<typename R = void, typename...Args>
FORCE_INLINE R stdcall(uintptr_t addr, Args...args)
{
	using pointer_type = R(__stdcall*)(extract_arg<Args>...);

	auto func = reinterpret_cast<pointer_type>(addr);
	
	return func(args...);
}

template<typename R = void, typename...Args>
FORCE_INLINE R call(uintptr_t addr, Args...args)
{
	using pointer_type = R(*)(extract_arg<Args>...);

	auto func = reinterpret_cast<pointer_type>(addr);
	
	return func(args...);
}

namespace checked {
template<typename Signature, typename...Args>
FORCE_INLINE auto thiscall(uintptr_t addr, Args&&...args)
{
	using pointer_type = typename Thiscall<Signature>::type;

	auto func = reinterpret_cast<pointer_type>(addr);
	
	return func(static_cast<Args&&>(args)...);
}
} // namespace checked
#endif//Gothic_Remake_Externals_H

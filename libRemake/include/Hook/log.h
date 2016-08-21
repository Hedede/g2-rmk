#ifndef HOOK_LOG_H
#define HOOK_LOG_H
#include <string>

void put(std::string s);

inline void put(char const* s)
{
	put(std::string(s));
}

template<typename T>
void put(T t)
{
	put(std::to_string(t));
}

template<typename... Args>
void print(Args... args)
{
	( put(args), ... );
}

template<typename... Args>
void println(Args... args)
{
	( put(args), ..., put("\n"));
}

#endif//HOOK_LOG_H

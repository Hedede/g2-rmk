#ifndef HOOK_CLOBBER
#define HOOK_CLOBBER

/*
 * Clobber Gothic functions with jump.
 * (Redirect call into our code)
 */

#include <cstdint>

namespace x86 {
enum Opcode {
	jmp = 0xFF,
};

enum Register {
	eax,
	ecx,
	edx,
	ebx,
	esp,
	ebp,
	esi,
	edi
};
}

void make_jump(char* out, uintptr_t addr, x86::Register reg)
{
	*out++ = 0xB8 + reg;
	*out++ = (addr >> 0)  & 0xFF;
	*out++ = (addr >> 8)  & 0xFF;
	*out++ = (addr >> 16) & 0xFF;
	*out++ = (addr >> 24) & 0xFF;
	*out++ = x86::jmp;
	*out++ = 0xE0 | reg;
}

#if 0
int test(int& x)
{
	++x;
	++x;
	++x;
	++x;
	++x;
	++x;
}

#include <iostream>
int tost(int& x)
{
	std::cout << x << "\n";
}

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
int main()
{
	int x = 0;

	void* mem;
	mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if ((uintptr_t)mem == -1)
		return 1;


	using fptr = void(*)(int&);
	fptr fun = (fptr)mem;


	
	std::cout << x << "\n";
	make_jump((char*)mem, (uintptr_t)test, x86::eax);
	fun(x);
	make_jump((char*)mem, (uintptr_t)tost, x86::eax);
	fun(x);
}
#endif

#endif

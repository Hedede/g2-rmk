#ifndef HOOK_CLOBBER
#define HOOK_CLOBBER

/*
 * Clobber Gothic functions with jump.
 * (Redirect call into our code)
 */

#include <cstdint>

namespace x86 {
enum Opcode {
	mov_i8 = 0xB0,
	mov_i  = 0xB8,
	retn   = 0xC3,
	jmp    = 0xFF,
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
} // namespace x86

namespace as {
uint8_t modrm(uint8_t mod, uint8_t reg, uint8_t rm)
{
	mod &= 0b11;
	reg &= 0b111;
	rm  &= 0b111;
	return (mod << 6) | (reg << 3) | (rm);
}

void u32(char*& out, uint32_t value)
{
	*out++ = (value >> 0)  & 0xFF;
	*out++ = (value >> 8)  & 0xFF;
	*out++ = (value >> 16) & 0xFF;
	*out++ = (value >> 24) & 0xFF;
}

void mov_u32(char* out, x86::Register reg, uint32_t value)
{
	*out++ = x86::mov_i + reg;
	u32(out, value);
}

void jump_m(char* out, uintptr_t addr)
{
	*out++ = x86::jmp;
	*out++ = modrm(0b00, 0b100, 0b101);
	u32(out, addr);
}

void jump_r(char* out, x86::Register reg)
{
	*out++ = x86::jmp;
	*out++ = modrm(0b11, 0b100, reg);
}

void retn(char* out)
{
	*out = x86::retn;
}
} // namespace as


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

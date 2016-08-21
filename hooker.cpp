#include <windows.h>

void dohook()
{
	auto ptr = LoadLibrary(TEXT("testlib.dll"));
	auto mod = GetModuleHandle(TEXT("testlib.dll"));
	int(*fun)()  = (int(*)())GetProcAddress(mod, TEXT("aw_main"));
	fun();
}
int main()
{
	dohook();
}

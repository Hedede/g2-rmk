#include <windows.h>
#include <debugapi.h>
#include <dbghelp.h>

#include <Logging/Log.h>

void wait_for_debugger()
{
	while( !::IsDebuggerPresent() )
		;
}


int __stdcall enum_lines_callback(
	PSRCCODEINFO LineInfo,
	PVOID UserContext)
{
	g2::Log(LineInfo->Obj, LineInfo->FileName, LineInfo->LineNumber);
	return true;
}

void read_symbols()
{
	DWORD  error;

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

	HANDLE curProcess = GetCurrentProcess();

	if (!SymInitialize(curProcess, NULL, TRUE))
	{
		error = GetLastError();
		g2::Log("SymInitialize returned error", error);
		return;
	}

	if (!SymEnumSourceLines(curProcess, 0x400000, NULL, NULL, 0, 0, enum_lines_callback, NULL))
	{
		error = GetLastError();
		g2::Log("SymEnumSourceLines returned error", error);
		return;
	}
}

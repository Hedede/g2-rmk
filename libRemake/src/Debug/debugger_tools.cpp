#include <debugapi.h>

void wait_for_debugger()
{
	while( !::IsDebuggerPresent() )
		;
}

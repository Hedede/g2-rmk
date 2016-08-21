enum zERROR_TYPE {
	zERR_TYPE_OK    = 0; /* [ungenutzt]        */
	zERR_TYPE_INFO  = 1; /* MEM_Info           */
	zERR_TYPE_WARN  = 2; /* MEM_Warn           */
	zERR_TYPE_FAULT = 3; /* MEM_Error          */
	zERR_TYPE_FATAL = 4; /* [ungenutzt]        */
}

const int zERR_ReportToZSpy     = zERR_TYPE_INFO;  //alles ab zERR_TYPE_INFO
const int zERR_ShowErrorBox     = zERR_TYPE_FAULT; //Messageboxen nur für Errors
const int zERR_PrintStackTrace  = zERR_TYPE_WARN; //Tracktrace printen für Warnings.

const int zERR_ErrorBoxOnlyForFirst   = 1; /* nie mehr als eine Error-Box anzeigen */
const int zERR_StackTraceOnlyForFirst = 0; /* nur für den ersten Error Stack Trace anzeigen */

class zERROR {
public:
	virtual ~zERROR();

	zSTRING GetFilterAuthors() const
	{
		return filter_authors;
	}

	zSTRING GetFilterFlagDescription() const
	{
		return "["_s + filter_flag + "]";
	}

	int GetFilterLevel() const
	{
		return filter_level;
	}

	zSTRING GetTargetDescription() const
	{
		zSTRING ret = "[";
		if (target & 1)
			ret += "SPY ";
		if (target & 2)
			ret += "FILE ";
		return ret + "]";
	}
private:
	void (*onexit)();

	zSTRING filter_authors;
	int     filter_flag;
	int     filter_level;

	int target;

	zERROR_TYPE ack_type;
	zFILE* log_file;
	zBYTE indent_depth;

	HWND     spyHandle;
	zCMutex* spyMutex;
};

zERROR zerr;


BOOL zERROR::SearchForSpy()
{
	spyHandle = FindWindowA(0, "[zSpy]");
	if ( spyHandle )
		zINFO(5,"B: ZERR: Spy found."); // 465

	return spyHandle != 0;
}

void zERROR::SendToSpy(zSTRING& msg)
{
	if ( spyHandle && spyMutex ) {
		spyMutex->Lock(-1);

		char const* m = msg.Data(); // ??
		size_t len = strlen(m);

		LPARAM data;
		SendMessageA(spyHandle, 0x4A, 0, &data);
		spyMutex->Unlock();
	}
}

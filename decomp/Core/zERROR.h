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
private:
	var int _vtbl;
	void (*onexit)();
	zSTRING filter_authors;
	var int	filter_flag;
	var int filter_level;
	var int	target;
	zERROR_TYPE ack_type;
	zFILE* log_file;
	zBYTE indent_depth;
	HWND spyHandle;
	zCMutex* spyMutex;
};

zERROR zerr;

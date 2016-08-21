enum zERROR_TYPE {
	zERR_TYPE_OK    = 0; /* [ungenutzt]        */
	zERR_TYPE_INFO  = 1; /* MEM_Info           */
	zERR_TYPE_WARN  = 2; /* MEM_Warn           */
	zERR_TYPE_FAULT = 3; /* MEM_Error          */
	zERR_TYPE_FATAL = 4; /* [ungenutzt]        */
};
using zERROR_LEVEL = zERROR_TYPE;

const int zERR_ReportToZSpy     = zERR_TYPE_INFO;  //alles ab zERR_TYPE_INFO
const int zERR_ShowErrorBox     = zERR_TYPE_FAULT; //Messageboxen nur für Errors
const int zERR_PrintStackTrace  = zERR_TYPE_WARN; //Tracktrace printen für Warnings.

const int zERR_ErrorBoxOnlyForFirst   = 1; /* nie mehr als eine Error-Box anzeigen */
const int zERR_StackTraceOnlyForFirst = 0; /* nur für den ersten Error Stack Trace anzeigen */

struct zERROR {
	virtual ~zERROR();

	static int Message(zSTRING& msg); // __stdcall
	static int Warning(zSTRING const& msg); // __stdcall
	static int Fatal(zSTRING const& msg); // __stdcall
	static int Fault(zSTRING const& msg); // __stdcall

	int Report(zERROR_TYPE type, int id, zSTRING const& msg, char levelPrio, char flag, int line, char const* file, const char* function);
	void Report(zERROR_LEVEL _type, int xId, zSTRING const& str_text, char levelPrio, char flag, int line, char const* file, const char* function)
	{
		Report(_type, xId, str_text, levelPrio, flag, line, file, function);
	}

	bool SearchForSpy();
	void SendToSpy(zSTRING& msg);
	void ShowSpy(int show);

	zSTRING GetFilterAuthors() const
	{
		return filter_authors;
	}

	zSTRING GetFilterFlagDescription() const
	{
		return "["_s + filter_flag + "]";
	}

	void SetFilterFlags(int flags)
	void SetFilterAuthors(zSTRING authors);
	void SetFilterLevel(int max_level);
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

	void SetTarget(int _target);
private:
	void (*onexit)();

	zSTRING filter_authors{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
	int     filter_flag  = 0;
	int     filter_level = -1;

	int target = 1;

	zERROR_TYPE ack_type = 0;
	zFILE* log_file = 0;
	zBYTE indent_depth = 0;

	HWND     spyHandle;
	zCMutex* spyMutex = new zCMutex();
};

zERROR zerr;
zQUEUE<zSTRING> zErrBlockStack;

#define zINFO(level, msg) if (zerr.GetFilterLevel() >= level) \
		zerr.Report(zERR_TYPE_INFO, 0, msg, level, 0, __LINE__, __FILE__, 0)
#define zWARNING(msg) zerr.Report(zERR_TYPE_WARNING, 0, msg, 0, 0, __LINE__, __FILE__, 0)
#define zFAULT(msg) zerr.Report(zERR_TYPE_FAULT, 0, msg, 0, 0, __LINE__, __FILE__, 0)
#define zFATAL(msg) zerr.Report(zERR_TYPE_FATAL, 0, msg, -1, 0, __LINE__, __FILE__, 0)


bool zERROR::SearchForSpy()
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

void zERROR::ShowSpy(int show)
{
	if ( filter_level <= 0 )
		return;

	zSTRING msg;
	if ( show ) {
		msg = "SHOW";
	} else {
		msg = "HIDE";
	}

	ATOM atom = GlobalAddAtomA(msg.c_str());
	SendMessageA(HWND_BROADCAST, WM_LOGCOMMAND, 0, atom);
	while ( GlobalDeleteAtom(atom) );
}

int zERROR::Message(zSTRING& msg)
{
	if ( SLOBYTE(zerr.filter_level) < 5 )
		return 0;
	return zerr.Report(zERR_TYPE_INFO, 0, msg, 5, 0,
		465, "P:\\dev\\g2addon\\release\\ZenGin\\_bert\\zError.cpp", 0);
}

int zERROR::Warning(zSTRING const& msg)
{
	return zerr.Report(zERR_TYPE_FATAL, 0, msg, 0, 0,
		473, "P:\\dev\\g2addon\\release\\ZenGin\\_bert\\zError.cpp", 0);
}

int zERROR::Fatal(zSTRING const& msg)
{
	return zerr.Report(zERR_TYPE_FATAL, 0, msg, -1, 0,
		483, "P:\\dev\\g2addon\\release\\ZenGin\\_bert\\zError.cpp", 0);
}

int zERROR::Fault(zSTRING const& msg)
{
	return zerr.Report(zERR_TYPE_FAULT, 0, msg, 0, 0,
		488, "P:\\dev\\g2addon\\release\\ZenGin\\_bert\\zError.cpp", 0);
}

void zERROR::SetFilterFlags(int flags)
{
	filter_flag = flags;
	zINFO(0,"B: ERR: SetFilterFlags   " + GetFilterFlagDescription()); //497
}

void zERROR::SetFilterAuthors(zSTRING authors)
{
	filter_authors = authors;
	zINFO(0, "B: ERR: SetFilterAuthors " + authors); //504
}

void zERROR::SetFilterLevel(int max_level)
{
	filter_level = max_level;
	zINFO(10, "B: ERR: SetFilterLevel   "_s + filter_level); // 517
}

void zERROR::SetTarget(int _target)
{
	this->target = _target;
	if ( _target & 2 && !log_file ) {
		char name[200];
		int nameSize = 200;

		// AUTHENTIC variable names
		GetUserNameA(&name, &nameSize);

		zSTRING username(name);
		username.Lower(&v18);

		log_file = new zFILE_FILE("c:\\errorlog_by_"_s + username + ".txt");
		log_file->Create();
	}

	zINFO(0, "B: ERR: SetTarget        " + GetTargetDescription()); // 539, _bert\\zError.cpp
}

void zERROR::Separator(zSTRING titel)
{
	if ( filter_level <= 0 )
		return;

	zSTRING msg = "---------------";
	if ( spyHandle ) {
		if ( spyMutex ) {
			spyMutex->Lock(-1);
			SendMessageA(spyHandle, 0x4Au, 0, &msg);
			spyMutex->Unlock();
		}
	}
	if ( titel.Length() > 0 ) {
		zINFO("          " + titel); // 445,
		Separator("");
	}
}

void zERROR::BlockBegin(char *file, int line)
{
	if ( indent_depth >= 255 ) {
		zWARNING("B: ERR: indent-depth out of bound (>255)."); // 412
	} else {
		zErrBlockStack.Push(new zSTRING(file));
		++indent_depth;
	}
}

void zERROR::BlockEnd(char *file, int line)
{
	if ( !indent_depth || zErrBlockStack.Free() ) {
		zWARNING("B: ERR: BLOCK-END unexpected: "_s + file + " line " + line); // 421
	} else {
		zSTRING* front = zErrBlockStack.Pop();
		if (!front)
			front = zSTR_EMPTY;
		if ( front == file ) {
			--indent_depth;
		} else {
			zWARNING("B: ERR: BLOCK-END in " + *front + " missed: at BLOCK-END in " + file + " line " + line); // 427
		}
	}
}

int zERROR::Report(zERROR *this, zERROR_TYPE type, int id, zSTRING const& msg, char levelPrio, char flag, int line, char *file, const char *function)
{
	if ( filter_level < 0 && type != zERR_TYPE_FATAL )
		return id;

	_msg = msg;

	bool colon =  msg.Length >= 2 && msg[1] == ':';
	if ( type == zERR_TYPE_MESSAGE ) {
		if ( !colon || levelPrio > this->filter_level )
			goto END;

		zSTRING author = msg[0];
		if ( filter_authors.Search(0, author, 1u) < 0 )
			goto END;
	}

	zSTRING typeStr;
	switch ( type ) {
	case zERR_TYPE_MESSAGE:
		typeStr = "Info: ";
		break;
	case zERR_TYPE_WARNING:
		typeStr = "Warn: ";
		break;
	case zERR_TYPE_FAULT:
		typeStr = "Fault:";
		break;
	case zERR_TYPE_FATAL:
		typeStr = "Fatal:";
		break;
	default:
		typeStr = "?????:";
		break;
	}

	zSTRING level = levelPrio;
	auto aligned = level.Align(0, 2, spaceChar);
	if (aligned)
		aligned += 4;
	typeStr += aligned;
	typeStr += " ";
	auto chars = Spaces(2 * indent_depth);
	typeStr += msg.Inserted(2, chars);
	if (file) {
		typeStr += " .... ";
		zSTRING f = file;
		auto pos = SearchRev(f, "\\", 1);
		f.Delete(0, pos + 1);
		typeStr += "<" + f + ",#" + line;
		if (function)
			typeStr += ",";
			typeStr += "," + function;
		}
		typeStr += ">";
	}

	if ( target & 1 ) {
		if ( spyHandle ) {
			if ( spyMutex ) {
				spyMutex->Lock(-1);
				SendMessageA(spyHandle, 0x4Au, 0, &typeStr);
				spyMutex->Unlock();
			}
		}
	}
	if ( target & 2 && log_file ) {
		typeStr += "\n";
		log_file->Write(typeStr);
	}
	if ( type >= ack_type ) {
		if ( zrenderer )
			zrenderer->Vid_SetScreenMode(2);
		ErrorBox(msg);
	}

	if ( type == 4 )
		onexit();

	if ( (flag & 1) == 1 )
		BlockBegin(file, line);
	if ( (flag & 2) == 2 )
		BlockEnd(file, line);

	return id;
}

void [[noreturn]] default_exit()
{
	exit(1);
}

zERROR::zERROR()
{
	spyHandle = FindWindowA(0, "[zSpy]");;
	if ( spyHandle )
		zINFO(5,"B: ZERR: Spy found."); // 465
	if ( spyHandle )
		SetForegroundWindowEx(spyHandle);

	WM_LOGMSG = RegisterWindowMessageA("WM_LOGMSG");
	WM_LOGCOMMAND = RegisterWindowMessageA("WM_LOGCOMMAND");
	onexit = default_exit;

	char Buffer[12];
	_strtime(&Buffer);
	time(&Time2);
	return this;
}

void zERROR::Init(zSTRING cmdLine)
{
	cmdLine.Upper();

	if(cmdLine.Search(0, "-ZLOG:", 1) >= 0) {
		auto tmp = cmdLine;
		tmp.Delete("-ZLOG:", 3);
		tmp.Delete(" ", 4);

		auto word1 = tmp.PickWord(1, ",", ",");
		auto word2 = tmp.PickWord(2, ",", ",");
		auto word2 = tmp.PickWord(3, ",", ",");

		int target = 0;
		if (word2.Length() > 0) {
			if ( word2.Search( 0, "S", 1u) >= 0 )
				target = 1;
			if ( word2.Search(0, "F", 1u) >= 0 )
				target |= 2u;
		}
		SetTarget(target);

		if (word1.Length() > 0) {
			SetFilterLevel(word1.ToLong());
		} else {
			SetFilterLevel(-1);
		}

		if (word3.Length() > 0) {
			SetFilterAuthors(word3);
		} else {
			SetFilterAuthors("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		}
	}

	SearchForSpy();

	zSTRING msg = "START";
	ATOM atom = GlobalAddAtomA(msg.c_str());
	SendMessageA(HWND_BROADCAST, WM_LOGCOMMAND, 0, atom);
	while ( GlobalDeleteAtom(atom) ) ;
	ack_type = 4;
	if ( filter_level > 0 ) {
		ShowSpy(1);
		ack_type = 3;
	}
}

void zERROR::~zERROR()
{
	Separator("");
	if ( filter_level > 0 )
		ShowSpy(false);

	if ( log_file ) {
		if (log_file->IsOpened())
			log_file->Close;
		Delete(log_file);
	}
	Delete(spyMutex);

	while ( indent_depth > 0 ) {
		if ( zErrBlockStack.Free() )
			break;
		auto front = zErrBlockStack.front;
		auto data = zErrBlockStack.Pop();
		if (data)
			delete data;
	}
}

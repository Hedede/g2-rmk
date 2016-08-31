struct zCConDat {
	zCConDat(zSTRING const& h, zSTRING const& s, int t)
		: command(h), description(s), type(t)
	{
		command.Upper();
	}

	zCConDat(zSTRING const& h, zSTRING const& s, int t, void* a, int e)
		: command(h), description(s), type(t), adr(a), ele(e), extended(true)
	{
		command.Upper();
	}

	~zCConDat() = default;
private:
	zSTRING description;
	zSTRING command;
	int type;
	void *adr = nullptr;
	int ele   = 0;
	int extended = false;
	zCConDat *next;
};

const int zCON_MAX_EVAL = 15;

class zCConsole : public zCInputCallback {
	static zCConsole* cur_console;
	static zCList<zCConsole> active_consoles;
public:
	zCConsole();
	zCConsole(int x, int y, zSTRING const& name);

	void HandleEvent(int key) override;

	void SetAutoCompletion(bool b)
	{
		autocomplete = b;
	}

	bool HasAutoCompletion() const
	{
		return autocomplete;
	}


	void SetFocus()
	{
		if ( cur_console )
			cur_console->SetEnableHandleEvent(0);
		cur_console = this;
		SetEnableHandleEvent(1);
		SetHandleEventTop();
	}
	void RemoveFocus();
	void Hide();
	void Show();
	void Toggle();

	bool IsVisible() const
	{
		return conview && conview->ondesk;
	}

	void SetFlags(int flags)
	{
		conview->SetFlags(flags);
	}

	void SetPos(int x, int y)
	{
		px = x;
		py = y;
	}

	void SetDim(int x, int y)
	{
		lx = x;
		ly = y;
		dynsize = 0;
	}

	void SetWorld(zCWorld* world)
	{
		this->world = world;
	}

	zCWorld* GetWorld()
	{
		return world;
	}

	void SetParser(zCParser *parser)
	{
		this->cparser = parser;
	}

	zCParser* GetParser()
	{
		return this->cparser;
	}

	int EditInstance(int index, void* adr);
	int EditInstance(zSTRING const& name, void *adr)
	{
		if ( cparser ) {
			auto idx = cparser->GetIndex(name);
			return EditInstance(idx, adr);
		}

		return false;
	}

	void Register(zSTRING const& command, zSTRING const& desc);
	void Register(zSTRING const& command, zSTRING const& desc, int type);
	void InsertVar(zSTRING const& h, zSTRING const& s, int type, void* adr, int ele);

	using ChangedFunc = void( const zSTRING &s );
	void SetChangedFunc(ChangedFunc* func)
	{
		changedfunc = func;
	}

	using EvalFunc = zBOOL( const zSTRING& s, zSTRING& msg );
	void AddEvalFunc(EvalFunc* func)
	{
		if ( evalcount + 1 < zCON_MAX_EVAL )
			evalfunc[evalcount++] = func;
	}

private:
	void ShowInput();
	void AddSkip(int dir); // unused?

private:
	zSTRING id;
	zSTRING instr;
	zSTRING savemsg;

	zCArray<zSTRING> lastcommand;

	int lastCommandPos = 0;

	int px = 0;
	int py = 0;
	int lx = 0;
	int ly = 0;
	int showmax = 0;
	int skip = 0;

	int dynsize;
	int _var = false;
	int autocomplete = true;

	zList <zCConDat> list;

	zCView* conview = nullptr;

	int       evalcount = 0;
	EvalFunc* evalfunc[zCON_MAX_EVAL];
	ChangedFunc* changedfunc = nullptr;

	zCWorld*	 world   = nullptr;
	zCParser*	 cparser = nullptr;
	int edit_index;
	void*		 edit_adr;
};
zCConsole zcon; //0xAB3860


//-----------------------------------------------------------------------------
zCConsole::zCConsole(int x, int y, int name)
	: zCInputCallback()
{
	px = x;
	py = y;
	lx = 4000;
	ly = 0;
	id = name;
	dynsize = 1;

	active_consoles.Insert(this);
}

zCConsole::zCConsole()
	: zCInputCallback()
{
	dynsize = 0;

	active_consoles.Insert(this);

	Register("save infofile", "save infofile with current console-commands");
	Register("help", "list commands on spy");
}


//-----------------------------------------------------------------------------
int zCConsole::HandleEvent(int key)
{
	if ( !zCConsole::cur_console )
		return 0;

	int addch = key;
	int addkey = KEY_ESCAPE;

	int ret = 0;

	bool cmkey = in(key, KEY_ESCAPE, KEY_BACKSLASH, KEY_F2, KEY_PRIOR, KEY_NEXT, KEY_DELETE, KEY_INSERT);
	if ( cmkey ) {
		ret = 1;
		addkey = 0;
	} else {
		int shift = (zinput->KeyPressed(KEY_RSHIFT) || zinput->KeyPressed(KEY_LSHIFT)) ? KEY_LSHIFT : 0;
		int alt   = zinput->KeyPressed(KEY_RMENU) ? KEY_RMENU : 0;

		char ch = GetCharFromDIK(key, shift, alt);
		addch = ch;
		if ( ch >= ' ' && ch <= '\xFF' )
			ret = 1;
		if ( ch == '\b' || ch == '\r' ) {
			ret = 1;
			addkey = 0;
		}
	}
	if (ret)
		zCConsole::cur_console->AddChar(addch, addkey);
	return ret;
}

void zCConsole::RemoveFocus()
{
	if ( zCConsole::cur_console )
		zCConsole::cur_console->SetEnableHandleEvent(0);
	zCConsole::cur_console = 0;

	for (auto i = &active_consoles; i; i = i->next) {
		if (!i->Get()->IsVisible())
			continue;
		if (i->Get() != this) {
			i->Get()->SetFocus();
			break;
		}
	}

	if ( zcon.IsVisible() )
		zcon.SetFocus();

	if ( zCConsole::cur_console )
		zCConsole::cur_console->Update();
}

void zCConsole::Hide()
{
	if (IsVisible()) {
// private
void zCConsole::ShowInput(zCConsole *this)
		cur_console = 0; // yes, duplicate
		screen->RemoveItem(conview);

		RemoveFocus();
	}
}

void zCConsole::Toggle()
{
	if (IsVisible()) {
		Hide();
	} else {
		Show();
	}
}

void zCConsole::ShowInput(zCConsole *this)
{
	auto fontY = conview->FontY();

	auto line = ": " + instr;
	if ( zCConsole::cur_console == this )
		line.Insert("|"); // at end?

	conview->Print(0, 0x1FFF - fontY, line);
}

//-----------------------------------------------------------------------------
void zCConsole::Register(zSTRING const& command, zSTRING const& desc)
{
	auto condat = new zCConDat(command, desc, 0);
	list.Insert(condat);
}

void zCConsole::Register(zSTRING const& command, zSTRING const& desc, int param)
{
	auto condat = new zCConDat(command, desc, param);
	list.Insert(condat);
}

void zCConsole::InsertVar(zSTRING const& h, zSTRING const& s, int type, void* adr, int ele)
{
	_var = 1;
	auto condat = new zCConDat(h, s, type, adr, ele);
	list.Insert(condat);
}

//-----------------------------------------------------------------------------
void zCConsole::AddSkip(int dir)
{
	skip += dir;
	if (skip < 0)
		skip = 0;
	if ( showmax + skip > list.count )
		skip = list.count - showmax;
	return result;
}

struct zCConDat {
	zCConDat(zSTRING const& h, zSTRING const& s, int t)
		: command(h), description(s), type(t)
	{
		command.Upper();
	}

	zCConDat(zSTRING const& h, zSTRING const& s, int t, void* a, int e)
		: command(h), description(s), type(t), adr(a), ele(e), var(true)
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
	int var   = false;
	zCConDat *next;
};

const int zCON_MAX_EVAL = 15;

class zCConsole : public zCInputCallback {
	static zCConsole* cur_console;
	static zCList<zCConsole> active_consoles;
public:
	zCConsole();
	zCConsole(int x, int y, zSTRING const& name);
	virtual ~zCConsole();

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

	int AutoCompletion(zSTRING& cmd);

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

	static zBOOL EditFunc(zSTRING const& s, zSTRING& msg);

private:
	void ShowInput();
	zCConDat* Get(zSTRING const& what)
	zCConDat* GetBestMatch(zSTRING const& what)
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

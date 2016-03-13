const int zCON_MAX_EVAL = 15;
zCConsole* zcon; //0xAB3860

class zCConsole : public zCInputCallback {
	static zCConsole* cur_console;
public:
	void SetFocus()
	{
		if ( cur_console )
			cur_console->SetEnableHandleEvent(0);
		cur_console = this;
		SetEnableHandleEvent(1);
		SetHandleEventTop();
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

	void SetParser(CParser *parser)
	{
		this->cparser = parser;
	}

	zCParser* GetParser()
	{
		return this->cparser;
	}

private:
	zSTRING id;
	zSTRING instr;
	zSTRING savemsg;

	zCArray<zSTRING> lastcommand;

	int	    lastCommandPos;

	int px;
	int py;
	int lx;
	int ly;
	int showmax;
	int skip;

	int dynsize;
	int _var;
	int autocomplete;

	zList <zCConDat>	list;

	zCView* conview;

	int	evalcount;
	zBOOL (*evalfunc[zCON_MAX_EVAL])  ( const zSTRING &s, zSTRING &msg );
	void  (*changedfunc)	( const zSTRING &s );

	zCWorld*	 world;
	zCParser*	 cparser;
	int edit_index;
	void*		 edit_adr;
};

class zCConDat {
public:
	~zCConDat() = default;
private:
	zSTRING description;
	zSTRING command;
};

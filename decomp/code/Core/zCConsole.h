// _ulf/zConsole.cpp
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

zCConsole::~zCConsole()
{
	Hide();
	Delete(conview);
	list.DeleteList();
	active_consoles.Remove(this);
}


//-----------------------------------------------------------------------------
int zCConsole::HandleEvent(int key)
{
	if ( !zCConsole::cur_console )
		return 0;

	int addch = key;
	int ischar = KEY_ESCAPE;

	int ret = 0;

	bool cmkey = in(key, KEY_ESCAPE, KEY_BACKSLASH, KEY_F2, KEY_PRIOR, KEY_NEXT, KEY_DELETE, KEY_INSERT);
	if ( cmkey ) {
		ret = 1;
		ischar = 0;
	} else {
		int shift = (zinput->KeyPressed(KEY_RSHIFT) || zinput->KeyPressed(KEY_LSHIFT)) ? KEY_LSHIFT : 0;
		int alt   = zinput->KeyPressed(KEY_RMENU) ? KEY_RMENU : 0;

		char ch = GetCharFromDIK(key, shift, alt);
		addch = ch;
		if ( ch >= ' ' && ch <= '\xFF' )
			ret = 1;
		if ( ch == '\b' || ch == '\r' ) {
			ret = 1;
			ischar = 0;
		}
	}

	if (ret)
		zCConsole::cur_console->AddChar(addch, ischar);
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
		cur_console = 0; // yes, duplicate
		screen->RemoveItem(conview);
		RemoveFocus();
	}
}

void zCConsole::Show()
{
	skip = 0;
	showmax = list.count;
	if ( showmax > 24 )
		showmax = 24;
	if ( conview ) {
		if ( !conview->ondesk )
			screen->InsertItem(conview, 0);
	} else {
		auto lsy = 0x2000 / screen->FontY();
		if ( dynsize )
		{
			if ( showmax + 3 > lsy )
				showmax = lsy - 3;

			ly = (showmax + 3) * screen->FontY();
		} else {
			if ( lx < 0 )
				lx = 4000;
			if ( ly < 0 )
				ly = 2000;
		}

		conview = new zCView(px, py, px + lx, py + ly, 2);
		screen->InsertItem(conview, 0);
		conview->InsertBack("CONSOLE.TGA");
		conview->ClrFlags(512);
		conview->SetTransparency(150);
		conview->SetAlphaBlendFunc(2);
	}

	SetFocus();
	conview->DefPrintwin(0, 0, 0x1FFF, 0x1FFF - conview->FontY());
	Update();
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

void zCConsole::AddChar(int w, int character)
{
	if ( character ) {
		if ( w == ' ' && instr.IsEmpty() )
			return;

		instr += w;
		if ( autocomplete )
			AutoCompletion(instr);
		lastCommandPos = lastcommand.GetNum();
	} else {
		switch ( w ) {
		case KEY_7:
			if ( instr.Length() > 0 )
				instr.Delete(instr.Length() - 1, 1u);
			lastCommandPos = lastcommand.GetNum();
			break;
		case KEY_EQUALS:
			instr.Upper();
			if ( Evaluate(instr) ) {
				if ( _var )
					Update();

				if (instr != "")
					lastcommand.InsertEnd(instr);

				instr = "";
			}
			lastCommandPos = lastcommand.GetNum();
			break;
		case KEY_INSERT:
			if ( lastcommand.GetNum > 0 ) {
				if ( lastCommandPos - 1 >= 0 ) {
					instr = lastcommand[lastCommandPos--];
					instr.Lower();
				}
			}
			break;
		case KEY_DELETE:
			instr = "";
			lastCommandPos = lastcommand.GetNum();
			break;
		case KEY_ESCAPE:
		case KEY_BACKSLASH:
		case KEY_F2:
			Hide();
			changedfunc = 0;
			return;
		case KEY_PRIOR:                           // E
			AddSkip(-1);
			Update();
			break;
		case KEY_NEXT:
			AddSkip(1);
			Update();
			break;
		default:
			break;
		}
	}

	ShowInput();
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

zCConDat* zCConsole::Get(zSTRING const& what)
{
	auto cmd = what.PickWord(1, " ", zSTR_SKIP);
	cmd.Upper();

	for (auto i = list.root; i; i = i->next) {
		if (i->command == cmd)
			return i;
	}

	return nullptr;
}

zCConDat* zCConsole::GetBestMatch(zSTRING const& what)
{
	zSTRING cmd = what;
	cmd.Upper();

	for (auto i = list.root; i; i = i->next) {
		auto word = i->command->PickWord(1, " ", zSTR_SKIP);
		if (word.Search(0, cmd, 1) == 0)
			return i;
	}

	return nullptr;
}

int zCConsole::EditFunc(zSTRING const& s, zSTRING& mess)
{
	if ( zCConsole::cur_console ) {
		auto con = cur_console;
		auto pars = zCConsole::cur_console->cparser;

		auto word = s.PickWord(1, " ", zSTR_SKIP);
		word.Upper();

		if (word == "SAVE") {
			if ( pars->SaveInstance(con->edit_index, con->edit_adr) ) {
				mess = "Saved,";
			} else {
				mess = "Save failed.";
			}
		}
		return 1;
	}

	return 0;
}

void zCConsole::ShowHelp()
{
	zINFO(0,"U:CON: console commands:") // 786

	for ( auto i = list.root; i; i = i->next )
		zINFO(0,"U:CON: " + i->command + "   : " + i->description); // 789
}

void zCConsole::SaveInfoFile(zSTRING const& filename)
{
	zoptions->ChangeDir(DIR_SYSTEM);
	auto file = zfactory->CreateZFile(filename);
	file->Create2();

	for ( auto i = list.root; i; i = i->next ) {
		zSTRING line = "";
		for ( auto len = i->command.Length() / 8; len < 3; -- len)
			line += "\t";

		line += i->command +  ": " + i->description +  "\n";
		file->Write(line);
	}

	file->Close();
	delete file;
}

void zCConsole::Eval(zSTRING const& str)
{
	auto word = str.PickWord(1, " ", zSTR_SKIP);
	word.Upper();

	zCConDat* condat;
	for (auto i = list.root; i; i = i->next) {
		if (i->command == word) {
			condat = i;
			break;
		}
	}

	if (!condat || str.IsEmpty())
		return;

	while ( 1 ) {
		++v6;
		auto word1 = str.PickWord(i, " =", zSTR_SKIP);
		if ( word1 == "" )
			break;
		if ( word1 == "=" ) {
			if (condat->type < 2) {
				word = str.PickWord(i+1, " =", zSTR_SKIP);
			} else {
				auto pos = str.PickWordPos(3, , "=", zSTR_SKIP);
				if ( pos ) {
					while( *pos == ' ' ) ++pos;

					word = pos;
				}
			}

			switch ( condat->type ) {
			case 0:
				*condat->adr = word.ToLong();
				break;
			case 1:
				*condat->adr = word.ToFloat();
				break;
			case 2:
				*condat->adr = word; // str = adr
				break;
			case 3:
				**condat->adr = word; // str = *adr
				break;
			default:
				break;
			}

			if ( changedfunc )
				changedfunc(condat->command);
			return;
		}
	}
}

int zCConsole::EditInstance(int index, void *adr)
{
	if ( !adr )
		return 0;
	if ( !cparser )
		return 0;

	auto sym = cparser->GetSymbol(index);
	if (!sym || sym->GetType() != zPAR_TYPE_INSTANCE)
		return 0;

	auto base_idx = cparser->GetBaseClass(index);
	auto bclass   = cparser->GetSymbol(bclass_idx);

	dynsize = 1;
	lx = 4000;
	ly = 0;

	id = sym->name + "(" + bclass->name + ")";
	edit_index = index;
	edit_adr = adr;

	for (int i = 0; i < bclass->GetNumElements(); ++i) {
		auto sym =  cparser->GetSymbol(base_idx + i + 1);

		int type;
		switch (sym->GetType()) {
		case zPAR_TYPE_INT:
			type = 0;
			break;
		case zPAR_TYPE_FLOAT:
			type = 1;
			break;
		case zPAR_TYPE_STRING:
			type = 2;
			break;
		default:
			continue;
		}

		zSTRING name;
		if (auto pos = sym->name.Search(0, ".", 1); pos > 0)
			name = sym->name.Copied(pos + 1, -1);

		auto mem = adr + sym->GetOffset();
		auto ele = sym->GetNumElements();
		list.InsertLast(new zCConDat{"", name, type, mem, ele});
	}

	Register(zCConsole::EditFunc);
	Show();

	return 1;
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

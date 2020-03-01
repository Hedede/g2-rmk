// ZenGin/_ulf/zConsole.cpp
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
		line += "|";

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
	list.InsertLast(new zCConDat{h, s, type, adr, ele}); // 275
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

		InsertVar("", name, type, mem, ele);
	}

	Register(zCConsole::EditFunc);
	Show();

	return 1;
}

void zCConsole::Update()
{
	if ( list.Empty() )
		return;

	conview = this->conview;

	auto y = 0;
	auto fontY = conview->FontY();

	if (!conview)
		return;

	if ( id.data != "" ) {
		conview->PrintCX(0, id);
		y = fontY;
	}

	auto node = list.root;
	for (auto i = skip; i > 0; --i)
		node = node->next;

	for ( i = 0; (i < showmax) && node; ++i, node = node->next ) {
		if ( !node->adr )
			continue;

		zSTRING title = node->description;
		zSTRING ynk   = node->command;
		zSTRING value = "";

		switch ( node->param ) {
		case 0:
			value = *(int*)node->adr;
			break;
		case 1:
			value = *(float*)node->adr;
			break;
		case 2:
			value = *(zSTRING*)node->adr;
			break;
		case 3:
			if (auto str = *(zSTRING**)node->adr)
				value = *str;
			break;
		default:
			break;
		}

		int x = 0;
		if ( title != "" ) {
			conview->Print(x, y, title);
			x = 4000;
		}

		conview->Print(x, y, ynk);
		if ( node->ele <= 1 ) {
			conview->Print(6000, y, ":  " + value);
		} else {
			conview->Print(6000, y, ":  " + value + ":  ("+ node->ele +")");
		}
	}

	if ( dynsize )
		conview->Print(0, 0x1FFF - 2 * conview->FontY(), savemsg);

	ShowInput();
}

int zCConsole::Evaluate(zSTRING const& cmd)
{
	// this function was horrible spaghetti,
	// some stuff might be wrong
	savemsg.Clear();
	if ( cmd.IsEmpty() )
		return 0;

	if (_var) {
		zCConDat* node = list.root;
		if ( cmd->Search(0, "=", 1u) > 0 ) {
			auto word = cmd.PickWord(1, " ", zSTR_SKIP);
			word.Upper();

			for (; node; node = node->next) {
				if (node->command == word)
					break;
			}
		}

		if ( !node || node->ele != 1 ) {
			if ( _var ) {
				node = list.root;

				auto word = cmd.PickWord(1, " ", zSTR_SKIP);
				word.Upper();

				for (; node; node = node->next) {
					if (node->command == word)
						break;
				}

				if ( node && node->ele > 1 ) {
					type = node->type;
					adr  = node->adr;

					for (int i = 0; i < list.count; ++i) {
						if (!list.Get(i)->var)
							continue;
						auto node = list.Get(i);
						list.Remove(node); // lines different from Remove()!
						// (246, 255)
						delete node;
					}

					zSTRING h;

					dynsize = 1;
					lx = 4000;
					ly = 0;

					size_t size;
					if (in(type, 0,1))
						size = sizeof(int);
					else if (type == 2)
						size = sizeof(zSTRING);


					for (int i = 0; i < node->ele; ++i) {
						InsertVar(h, i, type, adr, 1);
						adr += size;
					}

					Hide();
					Show();
					return 1;
				}
			}
		}
	}

	auto* func = this->evalfunc;
	if ( func ) {
		zSTRING msg;
		if (cmd.Search("SAVE INFOFILE", 1u) >= 0 ) {
			SaveInfoFile("Readme_Console.txt");
			msg = "File saved (DIR_CONFIG : Readme_Console.txt)";
		} else if ( cmd == "HELP" || cmd =="HELP ") {
			ShowHelp();
		} else for(auto i = 0; i < zCON_MAX_EVAL; ++i) {
			if (!func) break;
			if (func(cmd, msg)) break;
			++func;
		}

		if ( msg != "" ) {
			if ( !dynsize ) {
				if ( conview ) {
					conview->Printwin(msg + "\n");
				}
			}
			msg.Clear();
		}
	}

	// possibly? it's too spaghetti to understand
	if (var)
		Eval(cmd);
	return 1;
}

int zCConsole::AutoCompletion(zSTRING& cmd)
{
	// заколебало разбирать этот код
	zSTRING result;
	zSTRING word2;
	zSTRING word3;
	zSTRING tmp = cmd;
	tmp.Upper();

	zCConDat* match;
	int matches = 0;
	bool found = false;
	for ( int i = 1; ; ++i ) {
		auto word = tmp.PickWord(i, " ", zSTR_SKIP);

		if ( word == "" )
			break;

		for (auto node = list.root; node; node = node->next) {
			for (auto j = 1; j < i; ++j) {
				auto c1 = node->command.PickWord(j, " ", zSTR_SKIP);
				auto c2 = tmp.PickWord(j, " ", zSTR_SKIP);

				if (c1 == c2)
					break;
			}

			if ( !_var && !node->adr && node->param == i && cparser ) {
				auto ac = cparser->AutoCompletion(word);
				if ( ac ) {
					word.Lower();
					cmd = str_1 + word;
				}

				return ac;
			}

			word2 = node->command.PickWord(" ", zSTR_SKIP);
			if (word2.Search(0, word, 1) && (word2 != word3)) {
				if (++mathches > 0)
					return false;
				word3 = word2;
				match = node;
			}
		}

		found = true;
		if ( matches != 1 )
			return false;
		matches = 0;

		word2 = ""; 
		word3.Lower();
		result += word3 + " ";
	}

	if ( found ) {
		cmd = result;
		if ( var ) {
			if ( match->ele == 1 )
				cmd += "= ";
		}
	}

	return found;
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

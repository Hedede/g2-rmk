static zCParser* zCParser::cur_parser;
struct zCParser * zCParser::GetParser()
{
	return zCParser::cur_parser;
}
/*
class zCParser {
	int linec; // 20A4h
	bool stop_on_error; //20E0h
};
*/
void zCParser::Reset()
{
	this->datsave = 1;
	this->linkingnr = -1;
	this->in_funcnr = -1;
	this->in_classnr = -1;
	this->stop_on_error = 1;
	this->instance = -1;
	this->instance_help = -1;
	this->debugmode = -1;
	this->self = 0;
	this->compiled = 0;
	this->parse_changed = 0;
	this->mergemode = 0;
	this->linec = 0;
	this->line_start = 0;
	this->ext_parse = 0;
	this->pc_start = 0;
	this->pc = 0;
	this->oldpc = 0;
	this->pc_stop = 0;
	this->oldpc_stop = 0;
	this->params = 0;
	this->stringcount = 10000;
	this->in_func = 0;
	this->in_class = 0;
	this->error = 0;
	this->errorline = 0;
	this->prevword_nr = 0;
	this->tree = 0;
	this->treenode = 0;
	this->pad = 0;
	this->labelcount = 0;
	this->labelpos = 0;
	this->msgfunc = 0;

	win_code = 0;
	progressBar = 0;

	treeload = 0;
	treesave = 0;

	memset(this->prevword_index, 0, sizeof(this->prevword_index));
	memset(this->prevline_index, 0, sizeof(this->prevline_index));

	for ( i = 0; i < this->files.NumInList(); ++i )
	{
		if ( files[i] )
			delete file[i];
	}
	
	files.Clear();
	symtab.Clear();
	strings.Clear();
	stack.Clear();
	datastack.Clear();

	auto newsym = new zCPar_Symbol(0xFFFFFFFF + "INSTANCE_HELP");
	sym->bitfield = 0x7001;
	symtab.Insert(newsym);
	instance_help = symtab.GetIndex(newsym);
}

void zCParser::Error(zSTRING& errmsg, int pos)
{
	zSTRING msg;

	if (linkingnr < 0) {
		if (pos < 0) {
			msg = errmsg
		} else {
			msg = errmsg + " ( line " + linec + " )"; 
			errorline = linec;
		}
	} else {
		msg = GetFileName(linkingnr) + ": " + errmsg;
		errorline = pos;
	}

	if (msgfunc)
		msgfunc(msg);

	msg = "U:PAR: " + msg;

	if (stop_on_error) {
		zERROR::Report(zerr, zERR_TYPE_FATAL, 0,
			       msg, -1, 0, 0x257,
			       "P:\dev\g2addon\release\ZenGin\_ulf\zParser.cpp", 0);
		sysExit();
	}

	zERROR::Message(msg);

	return 0;
}

void zCParser::Message(zSTRING& msg)
{
	if ( msgfunc )
	{
		this->msgfunc(msg);
	}
	zERROR::Message("U:PAR: " + msg);
}

zCPar_Symbol* zCParser::GetSymbol(zSTRING const& name)
{
	auto tmp = name;
	return symtab.GetSymbol(tmp.Upper());
}

zCPar_Symbol* zCParser::GetSymbol(int index)
{
	return symtab.GetSymbol(index);
}

zSTRING zCParser::GetText(int symidx, int idx)
{
	zSTRING text;
	auto sym = GetSymbol(symidx);
	if ( sym && (sym->bitfield & 0xF000) == 0x3000 )
		sym->GetValue(&text, idx);
	return text;
}

int zCParser::ReadVarType()
{
	ReadWord(this->aword);
	if (aword == "INT") {
		return zPAR_TYPE_INT;
	} else if (aword == "FLOAT") {
		return zPAR_TYPE_FLOAT;
	} else if (aword == "STRING") {
		return zPAR_TYPE_STRING;
	} else if (aword == "FUNC") {
		return zPAR_TYPE_FUNC;
	} else {
		int symindex = symtab.getIndex(aword);
		auto sym = GetSymbol(symindex);
		if ( symbol && (symbol->bitfield & 0xF000) == 0x4000 )
		{
			return = symindex + 7;
		}
	}
	Error(ParErr_SyntaxError + aword, 0);
	return -1;
}

int zCParser::SaveDat(zSTRING& fileName)
{
	if (this->datsave) {
		zSTRING msg = "Saving " + fileName;

		if (this->msgfunc)
			this->msgfunc(msg);

		zERROR::Message("U:PAR: " + msg);

		zoptions->ChangeDir(0x13);

		zFILE file = zfactory->CreateZFile(fileName);
		file->Create();

		file->Write(&tree_version, 1);
		this->symtab->Save(file);
		this->stack->Save(file);

		file->Close();

		delete file;
	}
	return 0;
}

int zCParser::LoadDat(zSTRING& fileName)
{
	int result = 0;

	zoptions->ChangeDir(0x13);

	zSTRING msg = "Loading " + fileName;

	if (msgfunc)
		msgfunc(msg);

	zERROR::Message("U:PAR: " + msg);

	zFILE* file = zfactory->CreateZFile(fileName);
	file->Open(0);

	int version;
	file->Read(&version, 1);

	if (version == tree_version) {
		symtab.load(file);
		stack.load(file):

		zSTRING tmp = '\xFF' + "INSTANCE_HELP";
		instance_help = symtab.GetIndex(tmp);
	} else {
		zSTRING msg = "Dat-File is not compatible. Please use the \"SRC\"-File.";
		Error(msg, 0):

		result = -1;
	}

	delete file;

	return result;
}

void zCParser::PushVarAddress(int adr)
{
	datastack.Push(adr);
	datastack.Push(0x41);
}

void zCParser::PushDataValue(int val)
{
	datastack.Push(val);
	datastack.Push(0x40);
}

int zCParser::PopDataValue()
{
	int result;

	result = datastack.Pop();
	if (v2 == 64)
	{
		result = datastack.Pop(v1);
	}
	else if (v2 == 65)
	{
		result = *(int*)datastack.Pop(v1);
	}
	else
	{
		result = 0;
	}
	return result;
}

float zCParser::PopFloatValue()
{
	int val;
	float result;

	val = datastack.Pop();
	if ( val == 64 )
	{
		result = datastack.PopFloat();
	}
	else if ( val == 65 )
	{
		result = *(float *)datastack.Pop();
	}
	else
	{
		result = 0.0;
	}
	return result;
}

zSTRING* zCParser::PopString()
{
	datastack.Pop();
	return (zSTRING*)datastack.Pop();
}

int zCParser::PopVarAddress()
{
	int result;

	result = datastack.Pop();
	if (result == 65)
	{
		result = datastack.Pop();
	}
	else
	{
		datastack.Pop();
		result = 0;
	}
	return result;
}

void zCParser::Execute(int op1, int op2, char opcode)
{
	switch (opcode) {
	case 0:
		return PushDataValue(op1 + op2);
	case 1:
		return PushDataValue(op1 - op2);
	case 2:
		return PushDataValue(op1 * op2);
	case 3:
		return PushDataValue(op1 / op2);
	case 4:
		return PushDataValue(op1 % op2);
	case 5:
		return PushDataValue(op1 | op2);
	case 6:
		return PushDataValue(op1 & op2);
	case 7:
		return PushDataValue(op1 < op2);
	case 8:
		return PushDataValue(op1 > op2);
	case 11:
		return PushDataValue(!!op1 || !!op2);
	case 12:
		return PushDataValue(!!op1 && !!op2);
	case 13:
		return PushDataValue(op1 << op2);
	case 14:
		return PushDataValue(op1 >> op2);
	case 15:
		return PushDataValue(op1 <= op2);
	case 16:
		return PushDataValue(op1 == op2);
	case 17:
		return PushDataValue(op1 != op2);
	case 18:
		return PushDataValue(op1 => op2);
	case 9:
		*(int*)op1  = op2;
		return;
	case 19:
		*(int*)op1 += op2;
		return;
	case 20:
		*(int*)op1 -= op2;
		return;
	case 21:
		*(int*)op1 *= op2;
		return;
	case 22:
		*(int*)op1 /= op2;
	}
}

void zCParser::DoStack(int entry)
{
	int debug = 0;
	stack.SetPopPos(entry);

	do {
		if (debugmode) {
			int pop_pos = stack.GetPopPos();
			ShowPCode(pop_pos, win_code, 1);

			zCOLOR black{0,0,0,0};

			zrenderer->Vid_Clear(&black, 3);

			screen->DrawItems();
			zrenderer->Vid_Blit(1,0,0);

			if (input->KeyPressed(KEY_F5)) {
				if (win_code)
					screen->RemoveItem(win_code);
				debugmode = 0;
			} else {
				while (zinput->KeyPressed(KEY_F10) ||
				       zinput->KeyPressed(KEY_F11)) {
					sysEvent();
				}
				do {
					sysEvent();
					debug = zinput->KeyPressed(KEY_F11);
				} while (!zinput->KeyPressed(KEY_F10) &&
				         !zinput->KeyPressed(KEY_F11));
			}

		}

		zBYTE opcode = stack.PopByte();

		Symbol* symbol;
		int adr;
		int val;

		switch (opcode) {
		case zPAR_TOK_PUSHINT:
			val = stack.PopInt();
			PushDataValue(val);
			continue;

		case zPAR_TOK_PUSHVAR:
			adr = stack.PopInt();
			symbol = symtab.GetSymbol(adr);
			PushVarAddress(adr);
			goto PushVar;

		case zPAR_TOK_PUSH_ARRAYVAR:
			adr = stack.PopInt();
			symbol = symtab.GetSymbol(adr);
			val = stack.PopByte();

			adr = symbol.GetDataAdr(val);
			PushVarAddress(adr);
			continue;

		case zPAR_OP_IS:
		case zPAR_OP_ISPLUS:
		case zPAR_OP_ISMINUS:
		case zPAR_OP_ISMUL:
		case zPAR_OP_ISDIV:
			adr = PopVarAddress();
			val = PopDataValue();
			Execute(adr, val, opcode);
			continue;

		case zPAR_OP_PLUS:
		case zPAR_OP_MINUS:
		case zPAR_OP_MUL:
		case zPAR_OP_DIV:
		case zPAR_OP_MOD:
		case zPAR_OP_OR:
		case zPAR_OP_AND:
		case zPAR_OP_LOWER:
		case zPAR_OP_HIGHER:
		case zPAR_OP_LOG_OR:
		case zPAR_OP_LOG_AND:
		case zPAR_OP_SHIFTL:
		case zPAR_OP_SHIFTR:
		case zPAR_OP_LOWER_EQ:
		case zPAR_OP_EQUAL:
		case zPAR_OP_NOTEQUAL:
		case zPAR_OP_HIGHER_EQ:
			op1 = PopDataValue();
			op2 = PopDataValue();

			Execute(op1, op2, opcode);
			continue;

		case zPAR_OP_UN_MINUS:
			val = PopDataValue();
			PushDataValue(val);
			continue;

		case zPAR_OP_UN_PLUS:
			val = -PopDataValue();
			PushDataValue(val);
			continue;

		case zPAR_OP_UN_NOT:
			val = !PopDataValue();
			PushDataValue(val);
			continue;

		case zPAR_OP_UN_NEG:
			val = ~PopDataValue();
			PushDataValue(val);
			continue;


		case zPAR_TOK_CALL: {
			 int tmp = debugmode;
			 debugmode = debug;

			 entry = stack.PopInt();
			 adr = stack.GetPopPos();

			 DoStack(entry);

			 stack.setPopPos(adr);

			 debugmode = tmp;
			 continue;
		 }

		case zPAR_TOK_CALLEXTERN: {
			 int tmp = debugmode;
			 debugmode = 0;

			 cur_parser = this;

			 adr = stack.PopInt();
			 symbol = symtab.GetSymbol(adr);

			 sym->GetStackPos(entry, 0);

			 symbol = zParSymbol::GetUseInstance();
			 adr = stack.getPopPos();

			 void(*func)() = (void(*func)())entry;
			 func();

			 stack.SetPopPos(adr);
			 zParSymbol::SetUseInstance(symbol);

			 debugmode = tmp;
			 continue;
		 }

		case zPAR_TOK_JUMP:
			 entry = stack.PopInt();
			 stack.setPopPos(entry);
			 continue;

		case zPAR_TOK_JUMPF:
			 entry = stack.PopInt();
			 val = PopDataValue();
			 if (!val)
				 stack.setPopPos(entry);
			 continue;

		case zPAR_TOK_SETINSTANCE:
			 adr = stack.PopInt();
			 symbol = symtab.GetSymbol(adr);
			 CPar_Symbol::SetUseInstance(symbol);
			 continue;

		case zPAR_TOK_ASSIGNSTR:
			 adr = PopVarAddress();
			 *(zSTRING*)adr = *PopString();
			 continue;

		case zPAR_TOK_ASSIGNSTRP:
			 adr = PopVarAddress();
			 (zSTRING*)*adr = PopString();
			 continue;

		case zPAR_TOK_ASSIGNFUNC:
			 adr = PopVarAddress();
			 *adr = PopDataValue();
			 continue;

		case zPAR_TOK_ASSIGNFLOAT:
			 adr = PopVarAddress();
			 *(float *)adr = *PopFloatValue();
			 continue;

		case zPAR_TOK_PUSHINST:
			 val = stack.PopInt();
			 datastack.Push(val);
			 continue;

		case zPAR_TOK_ASSIGNINST: {
			 adr = datastack.Pop();
			 Symbol* sym1 = symtab.GetSymbol(adr);

			 adr = datastack.Pop();
			 Symbol* sym2 = symtab.GetSymbol(adr);

			 val = sym2->GetOffset();
			 sym1->SetOffset(sym2);
			 continue;
		}
		case zPAR_TOK_RET:
			 return;
		default:
			 Error("Error DoStack.", 0);
			 continue;
		}
	} while(1);
}

void zCParser::GetParameter(int& out)
{
	out = PopDataValue();
}

void zCParser::GetParameter(float& out)
{
	if (datastack.Pop() == 64) {
		int tmp = datastack.Pop();
		out = *(float *)&tmp;
	} else {
		out = *(float *)datastack.Pop();
	}
}

void zCParser::GetParameter(zSTRING& out)
{
	out = *PopString();
}

int zCParser::GetBase(int index)
{
	auto* symbol = symtab.GetSymbol(index);
	if (symbol) {
		int type = (symbol->bitfield & zCPar_Symbol::bitfield_type) >> 12;
		if ( type == 6 || type == 7 ) {
			auto* parent = symbol->GetParent();
			return symtab->GetIndex(parent->name);
		}
	}
	return -1;
}

int zCParser::GetBaseClass(zCPar_Symbol* symbol)
{
	if (!symbol)
		return -1;

	if ((symbol->bitfield & 0xF000) == 0x7000 ||
	    (symbol->bitfield & 0xF000) == 0x6000)
		symbol = zCPar_Symbol::GetParent(symbol);

	if ( symbol )
		return symtab.GetIndex(symbol->name);
	return -1;
}

int zCParser::GetBaseClass(int index)
{
	auto* symbol = symtab.GetSymbol(index);
	return GetBaseClass(symbol);
}

zBOOL zCParser::MatchClass(int index, zSTRING const& name)
{
	int class_index = GetIndex(name);
	int base_index = GetBaseClass(index);

	return class_index >= 0 && class_index == base_index;
}

int zCParser::GetIndex(zSTRING const& name)
{
	zSTRING tmp = name;
	tmp.Upper();

	return symtab.GetIndex(tmp);
}

void zCParser::SetReturn(void *offset)
{
	datastack.Push(instance_help);
	auto* instance_help = symtab.GetSymbol(this->instance_help);
	if ( instance_help )
		instance_help->SetOffset(int(offset));
}

void zCParser::SetReturn(float val)
{
	datastack.Push(int(val));
	datastack.Push(zPAR_OP_PUSHVAR);
}

void zCParser::SetReturn(int val)
{
	datastack.Push(val);
	datastack.Push(zPAR_OP_PUSHVAR);
}

void zCParser::SetReturn(zSTRING* str)
{
	datastack.Push(str);
	datastack.Push(66);
}

void zCParser::PrevWord()
{
	pc = prevword_index[prevword_nr];
	linec = prevline_index[prevword_nr];
	prevword_nr = (prevword_nr - 1) & 0xF;
}

void zCParser::SolveLabels(zCPar_TreeNode* node)
{
	do {
		zPAR_TOK type = node->tok_type;
		if ( type == zPAR_TOK_JUMP || type == zPAR_TOK_JUMPF )
			stack.Set(node->stack_index, labelpos[node->label_index]);
		if ( node->right )
			SolveLabels(node->right);
		if ( node->left )
			SolveLabels(node->left);

		node = node->next;
	} while ( node );
}

zCPar_TreeNode* zCParser::CreateLeaf(char tok, zCPar_TreeNode *node)
{
	auto *result = new zCPar_TreeNode(tok);
	if ( node )
		node->next = result;
	return result;
}

// private:
zCPar_TreeNode* zCParser::PushTree(zCPar_TreeNode *node)
{
	while (node)
		node = zCParser::PushOnStack(node);
	return node;
}

void* zCParser::GetInstanceAndIndex(int& index)
{
	index = datastack.Pop();
	if ( idx <= 0 )
	{
		return 0;
	}
	else
	{
		return (void *)symtab.GetSymbol(index)->GetOffset();
	}
}

void zCParser::SetPercentDone(int percent)
{
	if ( progressBar )
		progress.BarSetPercent(percent, zSTR_EMPTY);
}

zBOOL zCParser::CreateInstance(int instanceId, void* mem)
{
	zCPar_Symbol *symbol = symtab->GetSymbol(instanceId);
	if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
		return 0;

	symbol->SetOffset(symbol, int(mem));

	int func_offset;
	zCPar_Symbol::SetUseInstance(symbol);
	symbol->GetStackPos(func_offset, 0);

	// Вызов конструктора instance
	DoStack(func_offset);
	return 1;
}

int zCParser::SetInstance(int instanceId, void* newAddr)
{
	auto sym = symtab.GetSymbol(instanceId);

	if ( sym && sym->type == zPAR_TYPE_INSTANCE )
	{
		sym->SetOffset(sym, newAddr);
		return 1:
	}

	return 0;
}

int zCParser::SetInstance(zSTRING const& instanceName, void* newAddr)
{
	auto sym = symtab.GetSymbol(instanceName);

	if (instanceName == "HERO") {
		if (!heroInst)
			heroInst = sym;
	}

	if ( sym && sym->type == zPAR_TYPE_INSTANCE )
	{
		sym->SetOffset(sym, newAddr);
		return 1:
	}

	return 0;
}

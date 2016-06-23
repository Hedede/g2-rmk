static zCParser* zCParser::cur_parser;
struct zCParser* zCParser::GetParser()
{
	return zCParser::cur_parser;
}

zCParser::zCParser(int symtab_size)
	: symtab{0}, strings{0}, stack{64000u}, datastack{}
{
	if ( symtab_size > 0 )
		symtab->SetSize(symtab_size);
	Reset();
}

zCParser::~zCParser()
{
	for (auto file : files)
		Delete(file);

	symtab.Clear();
	strings.Clear();
	stack.Clear();
	datastack.Clear();
}

void zCParser::Reset()
{
	datsave = 1;
	linkingnr = -1;
	in_funcnr = -1;
	in_classnr = -1;
	stop_on_error = 1;
	instance = -1;
	instance_help = -1;
	debugmode = -1;
	self = 0;
	compiled = 0;
	parse_changed = 0;
	mergemode = 0;
	linec = 0;
	line_start = 0;
	ext_parse = 0;
	pc_start = 0;
	pc = 0;
	oldpc = 0;
	pc_stop = 0;
	oldpc_stop = 0;
	params = 0;
	stringcount = 10000;
	in_func = 0;
	in_class = 0;
	error = 0;
	errorline = 0;
	prevword_nr = 0;
	tree = 0;
	treenode = 0;
	pad = 0;
	labelcount = 0;
	labelpos = 0;
	msgfunc = 0;

	win_code = 0;
	progressBar = 0;

	treeload = 0;
	treesave = 0;

	memset(this->prevword_index, 0, sizeof(this->prevword_index));
	memset(this->prevline_index, 0, sizeof(this->prevline_index));

	for ( i = 0; i < this->files.NumInList(); ++i ) {
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

void zCParser::CreateVarReferenceList(zSTRING const& className, zCArray<int>& refList)
{
	auto classIndex = zparser.GetIndex(className);
	if ( classIndex >= 0 ) {
		for (auto i = 0; i < symtab.GetNumInList(); ++i ) {
			if (!MatchClass(i, className))
				continue;
			refList.Insert(i);
		}
	}
}

int zCParser::ClearAllInstanceRefs()
{
	for (unsigned i = 0; i < symtab.GetNumInList(); ++i) {
		auto sym = symtab.GetSymbol(i);
		if ( sym && sym->GetType == zPAR_TYPE_INSTANCE )
			SetInstance(i, 0);
	}
	return 1;

}
int zCParser::ClearInstanceRefs(void* adr)
{
	for (unsigned i = 0; i < symtab.GetNumInList(); ++i) {
		auto sym = symtab.GetSymbol(i);
		if (!sym)
		       continue;
		if (sym->GetType() != zPAR_TYPE_INSTANCE)
		       continue;
		if (sym->GetInstanceAdr() == adr)
			SetInstance(i, 0);
	}
	return 1;
}

void zCParser::ResetWithVarReferenceList(zCArray<int>& refList, void* adr)
{
	for (auto idx : refList) {
		sym = symtab.GetSymbol(idx);
		if (!sym)
			continue;
		if ( sym->GetType() == zPAR_TYPE_INSTANCE)
			continue;
		if (sym->GetInstanceAdr() == adr)
			SetInstance(i, 0);
	}
}

int zCParser::ResetGlobalVars()
{
	for (auto i = 0; i < symtab.GetNumInList(); ++i ) {
		auto sym = symtab.GetSymbol(i);
		if (sym && sym->GetType() != zPAR_TYPE_INT)
		       continue;
		if (sym->HasFlag(0x3F))
			continue;
		for (auto j = 0; j < sym->ele; ++j)
			sym->SetValue(0, j);
	}
	return 1;
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
		zFATAL(msg); // 599, _ulf/zParser.cpp
		sysExit();
	}

	zERROR::Message(msg);

	return 0;
}

void zCParser::Message(zSTRING& msg)
{
	if ( msgfunc )
		msgfunc(msg);
	zERROR::Message("U:PAR: " + msg);
}


int zCParser::CheckClassSize(zSTRING& className, int size)
{
	int idx = GetIndex(className);
	return CheckClassSize(idx, size);
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

int zCParser::GetInt(int index, int array)
{
	int out = 0;
	auto sym = symtab.GetSymbol(index);
	if ( sym && sym->GetType() == zPAR_TYPE_INT)
		sym->GetValue(&out, array);
	return out;
}

zSTRING zCParser::GetText(int symidx, int idx)
{
	zSTRING text;
	auto sym = GetSymbol(symidx);
	if ( sym && sym->GetType() == zPAR_TYPE_STRING)
		sym->GetValue(&text, idx);
	return text;
}

zSTRING zCParser::GetText(zSTRING& name, int array)
{
	auto idx = symtab.GetIndex(name, array);
	return GetText(idx, 0);
}

int zCParser::ReadVarType()
{
	ReadWord(aword);
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
		if (sym && sym->GetType() == zPAR_TYPE_CLASS)
			return symindex + zPAR_TYPE_INSTANCE;
	}
	Error(ParErr_SyntaxError + aword, 0);
	return -1;
}

int zCParser::ReadFuncType()
{
	auto type = ReadVarType();

	if (type == -1)
		return -1;
	if (type >= zPAR_TYPE_INSTANCE)
		return zPAR_TYPE_INSTANCE;
	return type;
}

int zCParser::ReadArray()
{
	zSTRING word;
	ReadWord(word);

	int res = 0;
	if ( word == "[" ) {
		PrevWord();
	} else {
		res = ReadInt();

		auto exp = "]"_s;
		ReadWord(aword);
		if ( aword != exp )
			Error(ParErr_Expected +  "'" + exp + "'");
	}

	return res;
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
	int result = datastack.Pop();
	if (result == 64) {
		result = datastack.Pop(v1);
	} else if (result == 65) {
		result = *(int*)datastack.Pop(v1);
	} else {
		result = 0;
	}
	return result;
}

float zCParser::PopFloatValue()
{
	int val;
	float result;

	val = datastack.Pop();
	if ( val == 64 ) {
		result = datastack.PopFloat();
	} else if ( val == 65 ) {
		result = *(float *)datastack.Pop();
	} else {
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
	int result = datastack.Pop();
	if (result == 65) {
		result = datastack.Pop();
	} else {
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

zSTRING zCParser::GetSymbolInfo(int nr, int& typ, int& ele)
{
	auto sym = symtab.GetSymbol(nr);
	if ( sym ) {
		typ = sym->GetType();
		ele = sym->ele;
		return sym->name;
	}

	typ = 0;
	ele = -1;

	return "";
}

void zCParser::GetClassVarInfo(int index, zSTRING& varName, int& typ, int& ele)
{
	varName.Upper();

	auto csym = symtab.GetSymbol(index);
	typ = -1;
	ele = -1;
	if ( csym && csym->GetType() == zPAR_TYPE_CLASS ) {
		auto sym = symtab.GetSymbol(csym->name + "." + varName);
		if ( sym && sym->HasFlag(4) ) {
			typ = sym->GetType()
			ele = sym->ele;
		}
	}
}

zSTRING zCParser::GetClassVarInfo(cindex, int idx, int& typ, int& ele)
{
	auto csym = symtab.GetSymbol(cindex);
	typ = -1;
	ele = -1;
	if (csym && csym->GetType() == zPAR_TYPE_CLASS) {
		auto next = csym;
		for (unsigned i = idx; i > 0; --i) {
			if (next)
				next = next->GetNext();
		}

		zSTRING name;
		if ( next && next->HasFlag(4) ) {
			typ = next->GetType();
			ele = next->ele;
			name = next->name;

			auto pos = name.Search(0,".",1);
			return name.Copied(pos+1, 255);
		}

	}
	return "";
}

int zCParser::GetInstance(int classId, int pos)
{
	auto classSym = symtab.GetSymbol(classId);
	if (!classSym)
		return -1;

	for (auto i = pos; pos < symtab.GetNumInList(); ++i) {
		auto sym = symtab.GetSymbol(i);
		int type = sym->GetType();
		if (type != zPAR_TYPE_INSTANCE)
			continue;
		if ( sym->HasFlag(1))
			continue;
		if (sym->name.Search(0, ".", 1u) >= 0)
			continue;

		auto parent = sym->GetParent();
		if ( parent ) {
			int type = parent->GetType();
			if (type == zPAR_TYPE_PROTOTYPE)
				parent = parent->GetParent();
			if ( parent == classSym )
				return sym;
		}
	}

	return -1;
}

int zCParser::GetPrototype(int classId, int pos)
{
	auto classSym = symtab.GetSymbol(classId);
	if (!classSym)
		return -1;

	for (auto i = pos; pos < symtab.GetNumInList(); ++i) {
		auto sym = symtab.GetSymbol(i);
		auto type = sym->GetType();
		if (type == zPAR_TYPE_PROTOTYPE && sym->GetParent() == classSym )
			return i;
	}

	return -1;
}

int zCParser::GetBase(int index)
{
	auto* symbol = symtab.GetSymbol(index);
	if (!symbol)
		return -1;

	int type = symbol->GetType();
	if (type == zPAR_TYPE_PROTOTYPE || type == zPAR_TYPE_INSTANCE ) {
		auto* parent = symbol->GetParent();
		return symtab->GetIndex(parent->name);
	}
	return -1;
}


int zCParser::GetBaseClass(zCPar_Symbol* symbol)
{
	if (!symbol)
		return -1;

	auto type = symbol->GetType();
	if (type == zPAR_TYPE_INSTANCE)
		symbol = symbol->GetParent();
	if (type == zPAR_TYPE_PROTOTYPE)
		symbol = symbol->GetParent();

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

zCPar_TreeNode* zCParser::CreateFloatLeaf()
{
	bool pv = 0;
	ReadWord(aword);

	if (aword[0] == '-') {
		pv = 1;
		ReadWord(aword);
	} else if (aword[0] == '+') {
		ReadWord(aword);
	}

	if ( isdigit(aword[0]) ) {
		if ( pv )
			PrevWord();
		PrevWord();
		auto _float = ReadFloat();
		auto leaf = CreateLeaf(zPAR_TOK_FLOAT, 0);
		label_index = _float;
		stack_index = 1;

		return leaf;
	}

	if ( pv )
		PrevWord();

	auto leaf = CreateLeaf(zPAR_TOK_VAR, 0);
	leaf->name = aword;

	leaf->label_index = 0;
	leaf->stack_index = 1;

	return leaf;
}

zCPar_TreeNode* zCParser::CreateStringLeaf()
{
	ReadWord(aword);

	zSTRING word;
	if (aword == "\"") {
		PrevWord();
		ReadString(word);

		auto leaf = CreateLeaf(zPAR_TOK_NEWSTRING, 0);
		leaf->name = word;
		leaf->label_index = 0;
		leaf->stack_index = 3;
		return leaf;
	}

	ReadWord(word);
	PrevWord();

	if (word == "(") {
		auto tn = treenode;

		auto leaf = CreateLeaf(zPAR_TOK_SKIP,0);
		treenode = leaf;

		DeclareFuncCall(aword, 3);
		treenode = tn;

		return leaf;
	}

	auto leaf = CreateLeaf(zPAR_TOK_VAR,0);
	leaf->name = aword;
	leaf->label_index = 0;
	leaf->stack_index = 3;

	return leaf;
}

zCPar_TreeNode* zCParser::PushTree(zCPar_TreeNode *node)
{
	while (node)
		node = PushOnStack(node);
	return node;
}

zCPar_Symbol* zCParser::SearchFuncWithStartAddress(int startAddress)
{
	for (auto i = 0; i <= symtab.GetNumInList(); ++i) {
		int stackPos;
		auto sym = symtab.GetSymbol(i);
		auto type = sym->GetType();
		if ( type == zPAR_TYPE_FUNC || type == zPAR_TYPE_PROTOTYPE ) {
			stackPos = 0;
			sym->GetStackPos(&stackPos, 0);
			if ( startAddress == stackPos )
				return sym;
		}
	}
	return nullptr;
}

int zCParser::FindInstanceVar(zSTRING& name)
{
	auto pos = name.Search(0, ".", 1u);
	if ( pos <= 0 )
		return -1;

	auto varName = name.Copied(0, pos);
	name.Delete(0, pos + 1);

	auto idx = FindIndex(&varName);

	instance = idx;

	auto sym = symtab.GetSymbol(idx);

	auto base = GetBaseClass(sym);
	if (!base)
		return -1;

	name = parent->name + "." + name;
	return symtab.GetIndex(name);
}

int zCParser::FindIndex(zSTRING& varName)
{
	auto index = zCParser::FindInstanceVar(varName);
	if ( index >= 0 )
		return index;
	if ( in_func )
		index = symtab.GetIndex(in_func->name + "." + varName);
	if ( index >= 0 )
		return index;
	if ( in_class )
		index = symtab.GetIndex(in_class->name + "." + varName);
	if ( index >= 0 )
		return index;
	return symtab.GetIndex(varName);
}

void* zCParser::GetInstanceAndIndex(int& index)
{
	index = datastack.Pop();
	if ( idx <= 0 )
		return 0;
	return (void *)symtab.GetSymbol(index)->GetOffset();
}

void* zCParser::GetInstance()
{
	int index;
	return GetInstanceAndIndex(index);
}

void zCParser::SetPercentDone(int percent)
{
	if ( progressBar )
		progress.BarSetPercent(percent, zSTR_EMPTY);
}

zBOOL zCParser::CreatePrototype(int index, void* new_offset)
{
	zCPar_Symbol* sym = symtab->GetSymbol(index);
	if (!symbol || symbol->GetType() != zPAR_TYPE_PROTOTYPE)
		return false;

	int func_offset;
	sym->SetOffset(new_offset);
	sym->SetUseInstance();
	sym->GetStackPos(func_offset, 0);
	DoStack(func_offset);
	return 1;
}

zBOOL zCParser::CreateInstance(int instanceId, void* mem)
{
	zCPar_Symbol* symbol = symtab->GetSymbol(instanceId);
	if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
		return 0;

	symbol->SetOffset(symbol, int(mem));

	int func_offset;
	symbol->SetUseInstance();
	symbol->GetStackPos(func_offset, 0);

	// Вызов конструктора instance
	DoStack(func_offset);
	return 1;
}

int zCParser::SetInstance(int instanceId, void* newAddr)
{
	auto sym = symtab.GetSymbol(instanceId);

	if ( sym && sym->GetType() == zPAR_TYPE_INSTANCE ) {
		sym->SetOffset(newAddr);
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

	if ( sym && sym->GetType() == zPAR_TYPE_INSTANCE ) {
		sym->SetOffset(newAddr);
		return 1:
	}

	return 0;
}

zSTRING zCParser::GetInstanceValue(zSTRING& name, unsigned nr, void* adr, int array)
{
	name.Upper();
	auto idx = symtab.GetIndex(name);

	return GetInstanceValue(idx, nr, adr, array);
}

zSTRING zCParser::GetInstanceValue(int cindex, unsigned int nr, char *adr, int array)
{
	auto csym = symtab->GetSymbol(cindex);
	if (!csym || csym->GetType() != zPAR_TYPE_CLASS)
		return "";

	if (nr <= 0 || nr >= csym->ele)
		return "";

	auto next = csym;
	auto sym = next;
	for (auto i = 0; i < csym->ele; ++i) {
		next = next->GetNext();
		auto offs = next->GetOffset();
		switch (next->GetType()) {
		case zPAR_TYPE_FUNC:
			offs = next->GetOffset();
			sym = symtab.GetSymbol(*(int)(adr + offs));
			if (sym)
				return sym->name;
		default:
			continue;
		case zPAR_TYPE_INT:
			return *(int*)(adr + offs);
		case zPAR_TYPE_FLOAT:
			return *(float*)(adr + offs);
		case zPAR_TYPE_STRING:
			return "\"" + *(zSTRING*)(adr + offs) + "\"";
		}
	}

	return "";
}

void zCParser::FindNext(char *SubStr)
{
	auto pos = pc;
	prevword_nr = (prevword_nr + 1) & 0xF;
	prevword_index[prevword_nr] = pc;
	prevline_index[prevword_nr] = linec;

	pc = strstr(pc, SubStr);

	if (pc) {
		do {
			pos = strchr(pos,'\n');
			if (!pos)
				break;
			if (pos > pc)
				return;
			++pos;
			++linec;
			line_start = pc - pc_start;
		} while (pos && pos <= pc);
	} else {
		Error("Unexpected End of File.");
	}
}

void zCParser::AddClassOffset(zSTRING& name, int newOffset)
{
	auto sym = symtab.GetSymbol(name);
	if (sym && sym->GetType() == zPAR_TYPE_CLASS) {
		if (compiled) {
			if (sym->GetClassOffset() != newOffset)
				Error("Dat-File is not compatible (Classoffset changed) ! Please reparse !");
		} else {
			sym->SetClassOffset(newOffset);
			auto next = sym;
			for (unsigned i = 0; i < sym->ele; ++i) {
				next = next->GetNext();
				if (next->HasFlag(zPAR_FLAG_CLASSVAR)) {
					auto off = next->GetOffset();
					next->SetOffset(off + newOffset);
				}
			}
		}
	}
}

zCPar_TreeNode* zCParser::ParseExpression(int& tok, int prec)
{
	auto nexttok = tok;
	auto primary = Parse_Expression_Primary(tok);
	if ( primary ) {
		while (tok <= zPAR_OP_UN_NEG) {
			if ( Op_Prio[tok] <= prec )
				break;
			auto nexttok = GetNextToken();

			auto rhs = Parse_Expression(tok, Op_Prio[tok]);
			if (IsAssignmentOp(tok)  && primary->tok_type != zPAR_TOK_VAR)
				Error("Assignment : Left operand is not a lvalue", 0);

			auto leaf = CreateLeaf(tok);

			newnode->left = primary;
			newnode->right = rhs;
			primary = newnode;

			tok = nexttok;
		}
	}
	return primary;
}

int zCParser::IsValid(zSTRING& className, void *data, zSTRING& p)
{
	auto line = p + ";";
	zCPar_TreeNode* leaf = 0;

	className.Upper();
	auto sym = symtab.GetSymbol(className);
	if ( sym && sym->GetType() == zPAR_TYPE_CLASS ) {
		error = 0;
		zCPar_Symbol::SetUseInstanceAdr(data);

		pc = line.Data();
		pc_stop = line.Data() + line.Length() + 1;

		auto res = ParseExpression();
		if ( res && !error )
			leaf = EvalLeaf(res, 0);

		return leaf != 0;
	}

	return 0;
}

int zCParser::Reparse(zSTRING& fileName)
{
	unsigned i = 0;
	for (; i < files.GetNum(); ++i) {
		if (GetFileName(i) == fileName) {
			files[i]->enableTreeLoad = 0;
			ParseFile(fileName);
			files[i]->enableTreeLoad = 0;
			break;
		}
	}
	return i;
}


zCPar_TreeNode* zCParser::ParseExpressionEx(zSTRING& tok)
{
	oldpc_stop = this->pc_stop;
	oldpc = pc;

	char* data = tok.Data();

	pc = data;
	pc_stop = &data[tok.Length() + 1];
	ext_parse = 1;

	tok = GetNextToken();
	return Parse_Expression(tok, -1);
}

void zCParser::DeclareClass()
{
	auto lc = linec;
	auto ls = line_start;

	zSTRING word;
	ReadWord(word);

	params = 1;

	auto csym = new zCPar_Symbol;
	in_class = csym;
	csym->name = word;
	csym->SetType(zPAR_TYPE_CLASS);

	if ( !symtab.Insert(in_class) )
		Error(ParErr_Redefined + in_class->name);

	ParseBlock();
	in_class->SetLineData(in_class, lc, linec - lc+1, ls, pc - pc_start - ls+3);
	in_class->SetFileNr(>in_class, files.GetNum() - 1);

	in_class = 0;
	in_classnr = 0;
	params = 0;
}

void zCParser::DeclareReturn()
{
	if ( in_func ) {
		if ( in_func->HasFlag(2) ) {
			auto offs = in_func->GetOffset() - 2;
			if ( offs && offs == 1 ) {
				treenode->next = CreateStringLeaf();
				while (treenode->next)
					treenode = treenode->next;
			} else {
				auto tok = GetNextToken();
				treenode->next = Parse_Expression(tok, -1);

				if ( treenode->next )
					treenode = treenode->next->SeekEndTree();

				PrevWord();
			}
		}

		auto leaf = CreateLeaf(zPAR_TOK_RET, 0);

		if ( treenode )
			treenode->next = leaf;
		treenode = leaf;
	} else {
		Errpr("Unexpected 'return'");
	}
}

void zCParser::ShowPCodeSpy(zSTRING& name)
{
	auto sym = GetSymbol(name);
	if (!sym)
		return;

	int pos;
	sym->GetStackPos(pos, 0);
	stack.SetPopPos(pos);
	printf("U:PAR: * PSEUDO - CODE *");

	for (unsigned i = 0; i < 40; ++i) {
		if (stack.GetPopPos() < stack.GetDynSize())
			break;
		zSTRING cmd;
		GetNextCommand(cmd);
		zINFO("U:PAR: " + cmd);
	}
}

void zCParser::ShowPCode(int adr, zCView *win_code, int instack)
{
	auto ynum = 0x1FFF / win_code->FontY();

	win_code->ClrPrintwin();
	stack.SetPopPos(adr);
	win_code->ClrPrintwin();

	zSTRING s = ":: ";
	if (in_func)
		s += in_func->name;
	s += " ::\n";

	win_code->Printwin(s);

	zSTRING cmd;
	while ( stack.GetPopPos() < stack.GetDynSize() ) {
		GetNextCommand(cmd);


		win_code->Printwin(cmd + "\n");

		if (--ynum < 0)
			break;
	}

	stack.SetPopPos(adr);
}


void zCParser::ShowCode(int index)
{
	auto pos = 0;
	auto sym = symtab.GetSymbol(index);
	if ( sym ) {
		if (sym->GetType() == zPAR_TYPE_FUNC ||
		    sym->GetType() == zPAR_TYPE_INSTANCE)
			sym->GetStackPos(&pos, 0);
	}

	auto fonty = screen->FontY();

	if ( !win_code ) {
		win_code = new zCView(0, 6*fonty, 8191, 10*fonty+6*fonty+2*fonty, 2);

		screen->InsertItem(win_code, 0);
		ClrFlags(win_code, 512);
	}

	if ( !win_code->ondesk )
		screen->InsertItem(win_code, 0);

	debugmode = 1;
	ShowPCode(pos, win_code, 0);
}

void zCParser::SetInfoFile(zCList<zSTRING>* funcList, zSTRING const& fileName)
{
	add_funclist = funcList;
	add_filename = fileName;
	add_created = 0;
}

int zCParser::IsInAdditionalInfo(zSTRING const& name)
{
	for (auto func : add_funclist) {
		func.Upper();
		if (func == name)
			return true;
	}
	return false;
	if ( !this->add_funclist.next )
		return 0;
}

int zCParser::AutoCompletion(zSTRING& str)
{
	// messy original, was lazy
	zSTRING res;
	int found = 0;
	for (unsigned i = 0; i < symtab.GetNumInList() <= 0; ++1) {
		auto sym = symtab.GetSymbol(i);
		if ( sym && sym->GetType() == zPAR_TYPE_INSTANCE ) {
			if ( !sym->name->Search(0, str, 1u) ) {
				++found;
				res = str;
				if ( found > 1 )
					break;
			}
		}
	}
	return found == 1;
}

void zCParser::DefineExternalVar(zSTRING& name, void* adr, short type, uint16_t ele)
{
	auto sym = new zCPar_Symbol(name);
	sym->name.Upper();
	sym->type = type;
	sym->ele  = ele;
	sym->SetFlag(9);
	sym->flags |= zPAR_FLAG_ALLOC_SPACE;

	if ( symtab.Insert(sym) ) {
		sym->SetDataAdr(adr);
		return;
	}

	if ( compiled ) {
		auto name = sym->name;
		delete sym;
		sym = GetSymbol(name);
		sym->SetDataAdr(adr);
		return;
	}

	Error("Redefined External Var. " + name, 0);
}


void zCParser::MergeFile(zSTRING& name)
{
	mergemode = 1;
	error = 0;
	treesave = 0;


	Parse(name);
	if ( !error ) {
		CreatePCode();

		for (auto i = 0; i < symtab.GetNumInList(); ++i) {
			auto sym = symtab.GetSymbol(i);
			if (!sym || !sym->GetType() != zPAR_TYPE_INSTANCE)
				continue;
			if (!sym->HasFlag(zPAR_FLAG_MERGED))
				continue;
			if (!sym->GetOffset())
				continue;


			int stpos = 0;
			sym->GetStackPos(&stpos, 0);
			stpos += 5;
			zCPar_Symbol::SetUseInstance(sym);

			DoStack(stpos);

			Message("Instance " + sym->name + " merged.");
		}
	}

	treesave = 0;
	mergemode = 0;
}

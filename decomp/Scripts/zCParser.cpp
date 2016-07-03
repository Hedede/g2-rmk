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

zSTRING ParErr_UnknownVar = "Unknown identifier : ";

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

void zCParser::PrevWord()
{
	pc = prevword_index[prevword_nr];
	linec = prevline_index[prevword_nr];
	prevword_nr = (prevword_nr - 1) & 0xF;
}

void zCParser::ReadWord(zSTRING& word)
{
	while ( 2 ) {
		while ( 2 ) {
			i = 0;

			PrevWord();

			while ( pc < pc_stop ) {
				++pc
				if ( *pc > ' ' ) {
					if ( !strrchr(trenn, *pc) ) {
						::word[i++] = *pc;
						continue;
					}

					if ( i <= 0 )
						::word[i++] = *pc;
					else
						--pc;
					break;
				}

				if ( tk != '\n' ) {
					if (i > 0)
						break;
					continue;
				}

				++linec;
				line_start = pc - pc_start;
				if (i > 0)
					break;

				prevword_index[prevword_nr] = pc_next;
				prevline_index[prevword_nr] = linec;
			}

			if ( !i ) {
				if ( ext_parse ) {
					ext_parse = 0;
					pc = oldpc;
					pc_stop = oldpc_stop;
					continue;
				}
			}

			break;
		}

		::word[i] = 0;

		word = ::word;
		word.Upper();

		if (word == "/") {
			if (*pc = '/') {
				++pc;
				FindNext("\n");
				++pc;
				continue;
			}
			if (*pc == '*') {
				++pc;
				FindNext("*/");
				pc += 2;
				continue;
			}
		}

		break;
	}
}

void zCParser::Match(zSTRING& match)
{
	ReadWord(aword)
	if (aword != match)
		Error(ParErr_Expected + "'" + match + "'");
}

int zCParser::GetNextToken()
{
	static zSTRING Operators = "+-*/%|&<>=";

	ReadWord(aword);

	zSTRING tmpstr;
	auto c = aword[0];
	if (c >= 'A' && c <= 'Z' || c == '_' || c == '#' ) {
		ReadWord(tmpstr);
		PrevWord();

		--prevword_nr;
		if ( tmpstr == "(" )
			return zPAR_TOK_FUNC;

		return zPAR_TOK_VAR;
	}

	if (c >= '0' && c <= '9') {
		for (auto i = 1; i < aword.Length(); ++i ) {
			if (!isdigit(aword[i]))
				Error(ParErr_SyntaxError + aword, 0);
		}

		return zPAR_TOK_FLOAT;
	}

	// tried to turn into switch (it probably WAS a switch)
	// but goto throws me off
	auto search = Operators.Search(0, c, 1u);
	if ( search >= 0 ) {
		if ( c == '|' ) {
			if ( *pc == '|' ) {
				++pc;
				return zPAR_OP_LOG_OR;
			}
			goto LABEL_37;
		}
		if ( c == '&' ) {
			if ( *pc == '&' ) {
				++pc;
				return zPAR_OP_LOG_AND;
			}
			goto LABEL_37;
		}
		if ( c == '<' ) {
			if ( *pc == '<' ) {
				++pc;
				return zPAR_OP_SHIFTL;
			}
			if ( *pc == '=' )
			{
				++pc;
				return zPAR_OP_LOWER_EQ;
			}
		}
		if ( c != '>' ) {
LABEL_37:
			if ( *pc == '=' ) {
				if ( c == '=' ) {
					++pc;
					return zPAR_OP_EQUAL;
				}
				if ( c == '+' ) {
					++pc;
					return zPAR_OP_ISPLUS;
				}
				if ( c == '-' ) {
					++pc;
					return zPAR_OP_ISMINUS;
				}
				if ( c == '*' ) {
					++pc;
					return zPAR_OP_ISMUL;
				}
				if ( c == '/' ) {
					++pc;
					return zPAR_OP_ISDIV;
				}
			}
			return search;
		}
		if ( *pc == '>' ) {
			++pc;
			return zPAR_OP_SHIFTR;
		}
		if ( *pc == '=' ) {
			++pc;
			return zPAR_OP_HIGHER_EQ;
		}

		return search;
	}

	if ( c == '!' ) {
		if ( *pc == '=' ) {
			++pc;
			return zPAR_OP_NOTEQUAL;
		} else {
			++pc;
			return zPAR_OP_UN_NOT;
		}
	}

	if ( c == '~' ) {
		++pc;
		return zPAR_OP_UN_NEG;
	}
	if ( c == '(' ) {
		++pc;
		return zPAR_TOK_BRACKETON;
	}
	if ( c == ')' ) {
		++pc;
		return zPAR_TOK_BRACKETOFF;
	}
	if ( c == ';' ) {
		++pc;
		return zPAR_TOK_SEMIKOLON;
	}
	if ( c == ',' ) {
		++pc;
		return zPAR_TOK_KOMMA;
	}
	if ( c == '}' || tmpchar == '{' ) {
		++pc;
		return zPAR_TOK_SCHWEIF;
	}

	Error("Syntax Error.", 0);

	return zPAR_TOK_NONE;
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

		Match("]");
	}

	return res;
}

int zCParser::ReadInt()
{
	int sign = 0;

	ReadWord(aword);

	if (aword == "-" || aword == "+") {
		if (aword == "-")
			sign = 1;
		ReadWord(aword);
	}

	int result;
	if (!isdigit(aword[0])) {
		auto idx = FindIndex(aword);
		auto sym = symtab.GetSymbol(idx);
		if ( !sym ) {
			Error(ParErr_UnknownVar + aword);
			return -1;
		}

		if (sym->GetType() != zPAR_TYPE_INT || !sym->HasFlag(1) ) {
			Error("Expected integer constant : " + aword);
			return -1;
		}

		sym->GetValue(&result, 0);
		return result;
	}

	for (unsigned i = 1; i < aword.Length(); ++i) {
		if (!isdigit(aword[i])) {
			Error("Expected an Integer-Value :" + aword);
			return -1;
		}
	}

	auto result = aword.ToInt();

	return sign ? -result : result;
}

float zCParser::ReadFloat()
{
	float result;
	int sign = 0;

	ReadWord(aword);

	if (aword == "-" || aword == "+") {
		if (aword == "-")
			sign = 1;
		ReadWord(aword);
	}

	bool exponent = 0;
	bool stop = 0;
	unsigned i = 0;
	for (; i < aword.Length; ++i) {
		if (isdigit(aword[i]))
			continue;
		if (aword[i] == '.' || stop)
			break;
		stop = 1;
	}

	if (aword[i] == 'E') {
		exponent = 1;
		if (i != aword.Length() - 1)
			Error(ParErr_SyntaxError + aword, 0);
	} else {
		Error(ParErr_SyntaxError + aword, 0);
		return -1.0;
	}

	str = aword;
	if ( exponent ) {
		auto val = ReadInt();
		str += ""_s + val;
	}

	result = str.ToFloat();
	return sign ? -result : result;
}

void zCParser::ReadString(zSTRING& out)
{
	Match("\"");

	while (*pc != '"') {
		out += *pc;

		if ( pc >= pc_stop ) {
			Error("Unexpected End of File.", 0);
		} else {
			++pc;
		}
	}

	++pc;
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

void zCParser::DeclarePrototype()
{
	auto olinec = linec;
	auto olinec_start = line_start;

	zSTRING word;
	ReadWord(word);

	if ( isdigit(word[0]) )
		Error("Syntax Error. First letter is a digit in prototypename.", 0);

	Match("(");

	ReadWord(aword);

	auto sym = symtab.GetSymbol(aword);

	if (sym && sym->GetType() == zPAR_TYPE_CLASS ) {
		in_func = new zCPar_Symbol(word);
		in_func->SetType(zPAR_TYPE_PROTOTYPE);

		in_func->SetParent(sym);
		in_class = sym;

		if ( !symtab.Insert(in_func) )
			Error(ParErr_Redefined + in_func->name);

		Match(")");

		treenode = CreateLeaf(zPAR_TOK_INSTANCE, treenode);
		treenode->name = word;

		ParseBlock();
		in_func->SetLineData(olinec, linec - olinec + 1, olinec_start, pc - pc_start - olinec_start + 3);
		in_func->SetFileNr(files.GetNum() - 1);

		treenode = CreateLeaf(zPAR_TOK_RET, treenode);
		treenode = CreateLeaf(zPAR_TOK_INSTANCEEND, treenode);
	} else {
		Error(ParErr_NoValidClass, aword);
	}
}

void zCParser::DeclareVar(int isConst)
{
	zCPar_TreeNode* node, node2;
	zCPar_Symbol* classsym = nullptr;

RECURSE:
	auto type = ReadVarType();
	if (type >= zPAR_TYPE_INSTANCE) {
		classsym = GetSymbol(type - 7);
		type = 7;
		type = 7; // sic!
	}

	while ( 1 ) {
		int line = linec;
		int pos = line_start;

		zSTRING name;
		ReadWord(name);

		int ele = 1;
		ReadWord(aword);
		if (aword == "[") {
			auto ele = ReadInt();
			zASSERT(ele < 4096); // zParser_Lexer.cpp, 948
			Match("]");
		} else {
			PrevWord();
		}

		auto newsym = new zCPar_Symbol()

		newsym->SetName(name);
		newsym->SetType(type);
		newsym->ele = ele;
		newsym->SetParent(classsym);

		if ( isConst )
			newsym->SetFlag(zPAR_FLAG_CONST);

		if ( in_func ) {
			zSTRING name = in_func->name + "." + newsym->name;
			newsym->SetName(name);

			if (params) {
				zASSERT(in_func->ele < 4095); // 966
				++in_func->ele;

				switch (newsym->GetType()) {
				case zPAR_TYPE_INT:
					node = new zCPar_TreeNode(zPAR_TOK_VAR, 0);
					node->name = newsym->name;
					node->label_index = 0;
					node->stack_index = 2;

					node2 = CreateLeaf(zPAR_OP_IS, node);
					node2->next = treenode->next;
					treenode->next = node;

					if ( in_func->ele == 1 )
						treenode = node2;
					break;
				case zPAR_TYPE_STRING:
					node = new zCPar_TreeNode(zPAR_TOK_VAR, 0);
					node->name = newsym->name;
					node->label_index = 0;
					node->stack_index = 3;

					node2 = CreateLeaf(zPAR_TOK_ASSIGNSTR, node);
					node2->next = treenode->next;
					treenode->next = node;

					if ( in_func->ele == 1 )
						treenode = node2;
					break;
				case zPAR_TYPE_INSTANCE:
					node = new zCPar_TreeNode(zPAR_TOK_PUSHINST);
					node->name = newsym->name;
					node->label_index = symindex;
					node->stack_index = 7;

					node2 = CreateLeaf(zPAR_TOK_ASSIGNINST, node);
					node2->next = treenode->next;
					treenode->next = node;

					if ( in_func->ele == 1 )
						treenode = node2;

					break;
				case zPAR_TYPE_FLOAT:
					node = new zCPar_TreeNode(zPAR_TOK_VAR);
					node->name = newsym->name;
					node->label_index = 0;
					node->stack_index = 1;

					node2 = CreateLeaf(zPAR_TOK_ASSIGNFLOAT, node);
					node2->next = treenode->next;
					treenode->next = node;

					if ( in_func->ele == 1 )
						treenode = node2;

					break;
				case zPAR_TYPE_FUNC:
					node = new zCPar_TreeNode(zPAR_TOK_VAR);
					node->name = newsym->name;
					node->label_index = 0;
					node->stack_index = 5;

					node2 = CreateLeaf(zPAR_TOK_ASSIGNFUNC, node)
						node2->next = treenode->next;
					treenode->next = node;

					if ( in_func->ele == 1 )
						treenode = node2;
					break;
				default:
					Error("Parameters of this type are not supported.", 0);
					break;
				}
			}
		} else if ( in_class ) {
			newsym.name = in_class->name + "." + newsym->name;

			if ( params ) {
				++in_class->ele;
				newsym->SetParent(in_class);
				newsym->SetFlag(zPAR_FLAG_CLASSVAR);
			}
		}

		if ( !symtab.Insert(newsym) )
			Error(ParErr_Redefined + newsym->name, 0);

		if ( isConst ) {
			Match("=");

			if ( ele > 1 )
				Match("{");

			for (auto i = 0; i < ele;) {
				switch ( type ) {
				case zPAR_TYPE_INT:
					tok = GetNextToken();
					expr = Parse_Expression(tok, -1);
					newsym->SetValue(EvalLeaf(expr, 0), i);

					PrevWord();
					break;
				case zPAR_TYPE_FLOAT:
					value = ReadFloat();
					newsym->SetValue(value, i);
					break;
				case zPAR_TYPE_STRING:
					ReadString(str);
					newsym->SetValue(str, i);
					break;
				case zPAR_TYPE_FUNC:
					ReadWord(aword);
					idx = GetIndex(aword);
					if ( idx < 0 )
						Error(ParErr_UnknownVar + aword, 0);

					newsym->SetType(zPAR_TYPE_INT);
					newsym->SetValue(idx, i);
					newsym->SetType(zPAR_TYPE_FUNC);
				default:
					break;
				}

				if (++i != ele)
					Match(",");
			}

			if ( ii > 1 )
				Match("}");

			if ( type == zPAR_TYPE_FUNC )
				newsym->SetType(zPAR_TYPE_INT);
		}

		newsym->SetLineData(line, linec - line + 1, pos, pc - pc_start - pos + 3);
		newsym->SetFileNr(files.GetNum() - 1);

		ReadWord(aword);
		if ( aword == "," )
			break;

		if ( in_func ) {
			Match("VAR");
			goto RECURSE;
		}
	}

	PrevWord();
}

void zCParser::DeclareFunc()
{
	auto old_linec = linec;
	auto old_line_start = line_start;

	auto ftype = ReadFuncType();

	auto sym = new zCPar_Symbol();
	sym->SetType(zPAR_TYPE_FUNC);
	sym->SetFlag(zPAR_FLAG_CONST);

	if ( !symtab.Insert(sym) )
		Error(ParErr_Redefined + sym->name);

	treenode = CreateLeaf(zPAR_TOK_FUNC, treenode);
	in_func = sym;

	if ( ftype )
		sym->SetFlag(zPAR_FLAG_RETURN);

	sym->SetOffset(ftype);

	Match("(");

	ReadWord(aword);

	if (aword = "VAR") {
		params = 1;
		DeclareVar(0);
		params = 0;

		Match(")");
	} else {
		if (aword != ")") {
			Error("Expected ')'");
			return;
		}
	}

	ParseBlock();

	sym->SetLineData(old_linec, linec - old_linec + 1, old_line_start, pc - pc_start - old_line_start + 3);
	sym->SetFileNr(files.GetNum() - 1);

	in_func = 0;
	in_funcnr = 0;
	treenode = CreateLeaf(zPAR_TOK_FUNCEND, treenode);
}

void zCParser::DeclareInstance()
{
	zCArray<zSTRING> names;
	zCArray<zCPar_Symbol *> symbols;

	auto olinec = linec;
	auto olinec_start = line_start;

	zSTRING word;

	do {
		ReadWord(word);
		if ( isdigit(word[0]) )
			Error("Syntax Error. First letter is a digit. Tnstancename : ", 0);
		names.InsertEnd(word);
		ReadWord(aword);
	} while (word == ",");

	PrevWord();

	Match("(");

	ReadWord(aword);
	auto csym = symtab.GetSymbol(aword);
	if (!csym || !in(csym->GetType(), zPAR_TYPE_CLASS, zPAR_TYPE_PROTOTYPE)) {
		Error(ParErr_NoValidClass + aword, 0);
		return;
	}

	Match(")");

	in_class = csym;
	if (csym->GetType() == zPAR_TYPE_PROTOTYPE)
		in_class = csym->GetParent();

	symtab.GetLastSymbol();

	for (auto name : names) {
		auto sym = new zCPar_Symbol();
		sym->SetName(name);
		sym->SetType(zPAR_TYPE_INSTANCE);
		sym->ele = 0;
		sym->SetParent(csym);
		in_func = sym;

		if ( !symtab.Insert(sym) ) {
			if ( !mergemode ) {
				Error(ParErr_Redefined + sym->name);
				return;
			}

			Delete(sym);

			sym = symtab.GetSymbol(name);
			if (sym && sym->GetType() != zPAR_TYPE_INSTANCE) {
				Error(ParErr_Redefined + sym->name, 0);
				return;
			}

			sym->SetFlag(zPAR_FLAG_MERGED);
		}

		symbols.InsertEnd(in_func);

		treenode = CreateLeaf(zPAR_TOK_INSTANCE, treenode);
		treenode->name = name;
	}


	ReadWord(aword);
	PrevWord();

	if (aword != ";") {
		ParseBlock();
		in_func->SetFlag(1);
	}

	ReadWord(aword);
	PrevWord();

	if (aword != ";") {
		ParseBlock();
		in_func->SetFlag(1);
	}

	for (auto sym : symbols) {
		sym->SetLineData(olinec, linec - olinec + 1, olinec_start, pc - pc_start - olinec_start + 3);
		sym->SetFileNr(files.GetNum() - 1);
	}

	in_func = 0;
	in_class = 0;
	in_classnr = 0;

	treenode = CreateLeaf(zPAR_TOK_RET, treenode);
	treenode = CreateLeaf(zPAR_TOK_INSTANCEEND, treenode);

	sym->SetLineData(olinec, linec - olinec + 1, olinec_start, pc - pc_start - olinec_start + 3);
	sym->SetFileNr(files.GetNum() - 1);

	in_func = 0;
	in_class = 0;
	in_classnr = 0;

	treenode = CreateLeaf(zPAR_TOK_RET, treenode);
	treenode = CreateLeaf(zPAR_TOK_INSTANCEEND, treenode);
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

void zCParser::DeclareAssign(zSTRING& name)
{
	auto idx = FindIndex(name);
	if (idx < 0) {
		Error(ParErr_UnknownVar + name);
		return;
	}

	auto sym = symtab.GetSymbol(idx);
	auto type = sym->GetType();

	int tok;
	switch (type) {
	case zPAR_TYPE_FLOAT:
		idx = ReadArray();
		Match("=");
		ReadWord(aword);
		sym = GetSymbol(aword);
		if (sym && sym->GetType() == zPAR_TYPE_FUNC) {
			DeclareFuncCall(aword,1);
			treenode = CreateLeaf(zPAR_TOK_VAR, treenode);
			treenode->name = name;
			treenode->label_index  = idx;
			treenode->startAddress = 1;
		} else {
			PrevWord();
			treenode = CreateLeaf(zPAR_TOK_FLOAT, treenode);
			treenode->label_index = ReadFloat();
			treenode = CreateLeaf(zPAR_TOK_VAR, treenode);
			treenode->name = name;
			treenode->label_index  = idx;
			treenode->startAddress = 1;
		}
		treenode = CreateLeaf(zPAR_TOK_ASSIGNFLOAT, treenode);
		return;
	case zPAR_TYPE_INT:
		PrevWord();
		tok = GetNextToken();
		treenode->next = Parse_Expression(tok, -1);
		return;
	case zPAR_TYPE_STRING:
		idx = ReadArray();
		Match("=");
		treenode->next = CreateStringLeaf();
		while (treenode->next)
			treenode = treenode->next;
		treenode = CreateLeaf(zPAR_TOK_VAR, treenode);
		treenode->name = name;
		treenode->label_index = idx;
		treenode->stack_index = 3;
		treenode = CreateLeaf(zPAR_TOK_ASSIGNSTR, treenode);
		return;
	case zPAR_TYPE_FUNC:
		if ( sym->HasFlag(zPAR_FLAG_CONST) )
			DeclareFuncCall(name, 0);
		else
			DeclareAssignFunc(name);
		return;
	case zPAR_TYPE_INSTANCE:
		Match("=");
		ReadWord(aword);
		DeclareFuncCall(aword, 7);
		treenode = CreateLeaf(zPAR_TOK_PUSHINST, treenode);
		treenode->name = name;
		treenode->stack_index = 7;
		treenode = CreateLeaf(zPAR_TOK_ASSIGNINST, treenode);
	default:
		return;
	}
}

void zCParser::DeclareAssignFunc(zSTRING& name)
{
	auto idx = ReadArray();

	Match("=");

	ReadWord(aword);

	auto old_treenode = treenode;

	treenode = CreateLeaf(zPAR_TOK_PUSHINDEX, treenode);
	treenode->name = aword;

	treenode = CreateLeaf(zPAR_TOK_VAR, treenode);
	treenode->name = name;
	treenode->label_index = idx;
	treenode->stack_index = 5;

	old_treenode = treenode;

	treenode = CreateLeaf(zPAR_TOK_ASSIGNFUNC, treenode);
}

void zCParser::DeclareIf()
{
	int idx = -1;
	while ( 1 ) {
		auto tok = GetNextToken();

		treenode->next = Parse_Expression(&tok, -1);
		while (treenode->next)
			treenode = treenode->next;

		int pos = labelpos;
		treenode = CreateLeaf(zPAR_TOK_JUMPF, treenode);
		treenode->label_index = labelpos++;

		PrevWord();

		ParseBlock();
		ReadWord(aword);

		if (aword == "ELSE") {
			treenode = CreateLeaf(zPAR_TOK_LABEL, treenode);
			treenode->label_index = pos;
			if ( aword == ";")
				break;
			continue;
		}

		if ( idx < 0 ) {
			idx = labelpos;
			labelpos = idx + 1;
		}

		treenode = CreateLeaf(zPAR_TOK_JUMP, treenode);
		treenode->label_index = idx;

		ReadWord(aword);
		if (aword == "{") {
			treenode = CreateLeaf(zPAR_TOK_LABEL, treenode);
			treenode->label_index = pos;

			PrevWord();

			ParseBlock();
			ReadWord(aword);

			Match(";");

			break;
		}

		PrevWord();

		Match("IF");
	}


	if ( idx >= 0 ) {
		treenode = CreateLeaf(zPAR_TOK_LABEL, treenode);
		treenode->label_index = idx;
	}

	PrevWord();
}

int zCParser::ParseFile(zSTRING& fileName)
{
	for (auto file : files) {
		if (fileName == file.GetName()) {
			zWARNING("U:PAR: Ignoring " + fileName + " ( File already parsed )"); // 784, zParser.cpp
			return 0;
		}
	}

	fname = fileName;
	labelpos = 0;

	auto node = new zCPar_TreeNode();
	tree = node;
	treenode = node;
	node->tok_type = zPAR_TOK_SKIP;

	auto file = new zCPar_File();
	files.InsertEnd(file);
	file->fileNum = files.GetNum();
	file->enableTreeLoad = treeload;

	zDATE date;
	auto load = file->Load(fileName, &symtab, &date);
	if ( load != -1 ) {
		if ( load ) {
			Message(fileName + " : Load...");
			labelpos = file->labelcount;
		} else {
			Message(fileName + " : Parse...");

			auto nsym = symtab.GetNumInList();
			auto lsym = symtab.GetLastSymbol();

			linec = 1;
			line_start = 0;
			pc = file->startAddress;
			pc_start = file->startAddress;
			pc_stop = (file->fileSize + file->startAddress - 1);

			// NOT same as ParseBlock()
			do {
				zSTRING tmp;
				ReadWord(tmp);

				if (tmp == "CONST")
					DeclareVar(1);
				else if (tmp == "VAR")
					DeclareVar(0);
				else if (tmp == "FUNC")
					DeclareFunc();
				else if (tmp == "CLASS")
					DeclareClass();
				else if (tmp == "INSTANCE")
					DeclareInstance();
				else if (tmp == "PROTOTYPE")
					DeclarePrototype();
				else if (tmp == "")
					break;
				else
					Error(ParErr_SyntaxError + tmp, 0)

						Match(";");
			} while(pc < pc_stop);

			Delete(file->startAddress);

			file->labelcount = labelpos;

			Delete(file->tree);

			file->tree = this->tree;

			if ( treesave ) {
				if ( lsym ) {
					auto sym = lsym->GetNext();
					auto nsym2 = symtab.GetNumInList();
					file->SaveTree(nsym2 - nsym, sym, date);
				} else {
					auto sym = fsym->GetFirstSymbol();
					auto nsym2 = symtab.GetNumInList();
					file->SaveTree(nsym2, fsym, date);
				}
			}
		}
		return 0;
	}

	Error("File " + fileName + " not found.", 0);

	files.Remove(file);

	delete file;
	return -1;
}

void zCParser::ParseBlock()
{
	Match("{");

	zSTRING word;
	ReadWord(word);

	while (pc < pc_stop) {
		if (word == "CONST")
			DeclareVar(true);
		else if (word == "VAR")
			DeclareVar(false);
		else if (word == "RETURN")
			DeclareReturn();
		else if (word == "IF")
			DeclareIf();
		else if (word == "}")
			return;
		else
			DeclareAssign(word);

		Match(";");
	}
}

zCPar_TreeNode* zCParser::Parse_Expression_Primary(int& tok)
{
	zCPar_TreeNode* ret = nullptr;
	switch (tok) {
	case zPAR_TOK_FLOAT:
		ret = new zCPar_TreeNode(zPAR_TOK_FLOAT, aword.ToLong());
		tok = GetNextToken();
		break;
	case zPAR_TOK_FUNC:
		{
			auto otnode = treenode;
			auto otree = tree;

			ret = new zCPar_TreeNode(zPAR_TOK_SKIP, 0);

			treenode = ret;
			tree = ret;

			DeclareFuncCall(aword, zPAR_TYPE_INT);
			ret = tree;

			treenode = otnode;
			tree = otree;
		}
		break;
	case zPAR_TOK_BRACKETON:
		tok = GetNextToken();
		ret = Parse_Expression(tok, -1);
		if ( tok != zPAR_TOK_BRACKETOFF ) {
			Error("Missing ')'", 0);
			break;
		}
		tok = GetNextToken();

		break;
	case zPAR_OP_PLUS:
	case zPAR_OP_MINUS:
		tok = GetNextToken();
		ret = CreateLeaf(tok + zPAR_OP_UN_PLUS, Parse_Expression_Primary(tok));
		tok = GetNextToken();
		break;
	default:
		Error(ParErr_SyntaxError + aword);
		tok = GetNextToken();
		break;
	case zPAR_OP_UN_PLUS:
	case zPAR_OP_UN_MINUS:
	case zPAR_OP_UN_NOT:
	case zPAR_OP_UN_NEG:
		tok = GetNextToken();
		ret = CreateLeaf(tok, Parse_Expression_Primary(tok));
		tok = GetNextToken();
		break;
	case zPAR_TOK_VAR:
		{
			auto idx = 0;
			auto tmp = aword;
			ReadWord(aword);

			if (aword == "(") {
				ReadWord(aword);

			} else if (aword == "[") {
				idx = ReadInt();
				Match("]");
			} else {
				PrevWord();
			}

			ret = new zCPar_TreeNode(tok, idx);
			ret->name = tmp;
			ret->stack_index = 2;

			tok = GetNextToken();
			break;
		}
	}

	return ret;
}

zCPar_TreeNode* zCParser::Parse_Expression(int& tok, int prec)
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


void zCParser::Parse(zSTRING fileName)
{
	fileName.Upper();
	mainfile = fileName;

	compiled = 0;

	auto pos = fileName.Search(0,".SRC",1);

	if ( enableParsing ) {
		if (pos > 0)
			return ParseSource(fileName);

		if ( fileName.Search(0, ".D", 1u) > 0 )
			return ParseFile(fileName);
	}

	if (pos > 0)
		fileName.Overwrite(pos, ".DAT");


	if ( fileName.Search(0, ".DAT", 1u) > 0 ) {
		compiled = 1;

		zPATH path(fileName);
		return LoadDat(path.GetFilename() + ".DAT");
	}
	return -1;
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
	if (result == zPAR_TOK_PUSHINT) {
		result = datastack.Pop();
	} else if (result == zPAR_TOK_PUSHVAR) {
		result = *(int*)datastack.Pop();
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

void* zCParser::EvalLeaf(zCPar_TreeNode* node, int w1)
{
	void* evalleft, evalright;
	if ( node->tok_type <= zPAR_OP_UN_NEG ) {
		if ( node->tok_type >= zPAR_OP_UN_PLUS ) {
			evalleft = node;
			evalright = EvalLeaf(node->right, 0);
		} else {
			evalleft = EvalLeaf(node->left, node.tok_type);
			evalright = EvalLeaf(node->right, 0);
		}

LABEL_34:
		switch ( node->tok_type ) {
		case zPAR_OP_PLUS:
			return evalleft + evalright;
		case zPAR_OP_MINUS:
			return evalleft - evalright;
		case zPAR_OP_MUL:
			return evalleft * evalright;
		case zPAR_OP_UN_PLUS:
			return evalright;
		case zPAR_OP_DIV:
			return evalleft / evalright;
		case zPAR_OP_MOD:
			return evalleft % evalright;
		case zPAR_OP_OR:
			return evalright | evalleft;
		case zPAR_OP_AND:
			return evalright & evalleft;
		case zPAR_OP_LOWER:
			return evalleft < evalright;
		case zPAR_OP_HIGHER:
			return evalleft > evalright;
		case zPAR_OP_LOG_OR:
			return evalleft || evalright;
		case zPAR_OP_LOG_AND:
			return evalleft && evalright;
		case zPAR_OP_SHIFTL:
			return evalleft << evalright;
		case zPAR_OP_SHIFTR:
			return evalleft >> evalright;
		case zPAR_OP_LOWER_EQ:
			return evalleft <= evalright;
		case zPAR_OP_EQUAL:
			return evalleft == evalright;
		case zPAR_OP_NOTEQUAL:
			return evalleft != evalright;
		case zPAR_OP_HIGHER_EQ:
			return evalleft >= evalright;
		case zPAR_OP_IS:
			return *evalleft = evalright;
		case zPAR_OP_ISPLUS:
			return *evalleft += evalright;
		case zPAR_OP_ISMINUS:
			return *evalleft -= evalright;
		case zPAR_OP_ISMUL:
			return *evalleft *= evalright;
		case zPAR_OP_ISDIV:
			return *evalleft /= evalright;
		case zPAR_OP_UN_MINUS:
			return evalleft = -evalright;
		case zPAR_OP_UN_NOT:
			return !evalright;
		case zPAR_OP_UN_NEG:
			return ~evalright;
		default:
			Error("Code - Error in Tree-Expression !");
			break;
		}
		return evalleft;
	}

	if ( node->tok_type == zPAR_TOK_FLOAT )
		return node->label_index;
	if ( node->tok_type != zPAR_TOK_VAR ) {
		Error("Error in EvalLeaf.");
		evalleft = node;
		evalright = node;
		goto LABEL_34;
	}

	auto idx = FindIndex(node->name);
	auto sym = symtab.GetSymbol(idx);

	if ( !sym ) {
		Error("Unknown Var '" + node->name + "'.");
	}

	if ( w1 == zPAR_OP_IS || w1 >= zPAR_OP_ISPLUS && w1 <= zPAR_OP_ISDIV ) {
		return sym->GetDataAdr(node->label_index);
	} else if ( zCPar_Symbol::HasFlag(sym, zPAR_FLAG_CLASSVAR) ) {
		return *sym->GetDataAdr(node->label_index);
	}

	sym->GetValue(&node, node->label_index);
	return node;
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

void zCParser::CreatePCode()
{
	error = 0;

	unsigned timediff = 0;
	if (!compiled) {
		Message("Linking ...");

		auto time = sysGetTime();
		unsigned n = 0;
		for (auto file : files) {
			if (file->tree) {
				linkingnr = n;
				labelpos = file->labelcount;
				add_funclist.Reserve(file->labelcount);

				PushTree(file->tree);

				SolveLabels(file->tree);

				Delete(file->tree);
			}
		}
		auto time2 = sysGetTime();
		timediff = time2 - time;
	} else {
		Message("Linking skipped.");
	}

	linkingnr = -1;
	if ( error ) {
		Message("Linking failed.");
	} else {
		Message("Symbols   : "_s + symtab.GetNumInList());
		Message("Code Size : "_s + stack.GetDynSize() + " bytes.");
		Message("Linking ok ("_s + (timediff * 0.001) + "s).");
	}

	if ( !compiled && parse_changed ) {
		zPATH path{mainfile};
		SaveDat(path.GetFilename() + ".DAT");
	}

	self = GetSymbol("SELF");
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

void zCParser::DefineExternal(zSTRING& name, int (*func)(), int type, int first, ...)
{
	auto funcsym = new zCPar_Symbol();
	funcsym->SetName(name);
	funcsym->SetType(zPAR_TYPE_FUNC);
	funcsym->SetFlag(zPAR_FLAG_EXTERNAL|zPAR_FLAG_CONST);
	if ( type )
		funcsym->SetFlag(zPAR_FLAG_RETURN);
	funcsym->SetOffset(type);

	if ( !symtab.Insert(funcsym) ) {
		if ( !compiled ) {
			Error("Redefined External . ", 0);
			return;
		}

		auto name = funcsym->GetName();
		delete funcsym;
		funcsym = GetSymbol(name);
	}

	funcsym->SetStackPos((int)func, 0);
	if (compiled)
		return;

	for (auto arg = &first; *arg; ++arg) {
		auto argsym = new zCPar_Symbol();
		
		auto id = funcsym->ele++;
		argsym->name = funcsym->name + ".PAR" + id;
		argsym->SetType(*arg);
		argsym->SetParent(0);

		symtab.Insert(argsym);
	}
}

int retval;
int* zCParser::CallFunc(int funcIdx, ...)
{
	retval = 0;

	ayto sym = symtab.GetSymbol(funcIdx);

	if ( sym && sym->GetType() == zPAR_TYPE_FUNC ) {
		datastack.Clear();

		char* arg = (char*)&funcIdx;
		for (unsigned i = 0; i < sym->ele; ++i) {
			auto var = symtab.GetSymbol(funcIdx + i + 1);

			if (var->GetType() == 1) {
				arg += sizeof(float);

				float value = *(float*)arg;
				var->SetValue(value, 0);

				datastack.Push(value);
				datastack.Push(64);
				continue;
			}
			if (var->GetType() == 2) {
				arg += sizeof(int);

				int value = *(int*)arg;
				var->SetValue(value, 0);

				datastack.Push(value);
				datastack.Push(64);
				continue;
			}
			if (var->GetType() == 3) {
				arg += sizeof(zSTRING);

				zSTRING value = *(zSTRING*)arg;
				var->SetValue(value, 0);

				auto adr = sym->GetDataAdr(0);
				datastack.Push(adr);
				datastack.Push(65);
				continue;
			}

			Error("Function type not supported. (" + sym->name + ")", 0);
		}

		labelcount = funcIdx;

		sym->GetStackPos(&stackPos, 0);
		DoStack(stackPos);

		if (sym->HasFlag(zPAR_FLAG_RETURN) && in(sym->GetOffset(), 1,2))
			retval = PopDataValue();

		labelcount = -1;
	} else {
		Message("Warning : Engine calls an undefined function. (index : "_s + funcIdx + " )");
	}

	return &retval;
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

int zCParser::SaveDat(zSTRING& fileName)
{
	if (datsave) {
		Message("Saving " + fileName);

		zoptions->ChangeDir(DIR_COMPILED_SCRIPTS);

		zFILE file = zfactory->CreateZFile(fileName);
		file->Create();

		file->Write(&tree_version, 1);

		symtab.Save(file);
		stack.Save(file);

		file->Close();

		delete file;
	}
	return 0;
}

int zCParser::LoadDat(zSTRING& fileName)
{
	int result = 0;

	zoptions->ChangeDir(DIR_COMPILED_SCRIPTS);

	Message("Loading " + fileName);

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

int zCParser::CheckClassSize(int index, int size)
{
	auto csym = symtab.GetSymbol(index);
	if ( csym && csym->GetType() == zPAR_TYPE_CLASS ) {
		if ( csym->GetOffset() != size ) {
			auto msg1 = "Class - size not compatible : " + csym->name + "\n";
			auto msg2 = "CPP - size : "_s + size + "\n";
			auto msg3 = "Script - size : " + csym->GetOffset();
			Error(msg1 + msg2 + msg3);
		}
		return csym->GetOffset() == size;
	}
	return 0;

}

int zCParser::CheckClassSize(zSTRING& className, int size)
{
	int idx = GetIndex(className);
	return CheckClassSize(idx, size);
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

int zCParser::WriteAdditionalInfo(zSTRING& call, int zeile, int filepos)
{
	zSTRING currentDir;
	zFILE_FILE file();
	file.SetCurrentDir(currentDir);

	zoptions->ChangeDir(DIR_COMPILED_SCRIPTS);

	auto zfile = zfactory->CreateZFile(add_filename);

	if ( add_created ) {
		zfile->unk1 = 0;
		zfile->Open(1);
		zfile->Append();
	} else {
		zfile->Create();
		add_created = 1;
	}

	zSTRING str;
	str = "FILE   { " + fname + " }\n";
	zfile->Write(str);
	str = "SYMBOL { " + call +  " }\n";
	zfile->Write(str);
	str = "LINE   { "_s + zeile +  " }\n";
	zfile->Write(str);
	str = "FILEPOS{ "_s + filepos + " }\n\n";
	zfile->Write(str);

	zfile->Close();
	delete zfile;

	file.ChangeDir(0);
	return 1;
}

void zCParser::SaveInfoFile(zSTRING fileName)
{
	auto zfile = zfactory->CreateZFile(fileName);
	zfile->Create();

	zfile->Write("// ***  Externals  ***\n\n");

	for (unsigned i = 0; i < symtab.GetNumInList(); ++i) {
		auto sym = symtab.GetSymbol(i);
		if (sym->HasFlag(zPAR_FLAG_EXTERNAL)) {
			zSTRING name = "FUNC ";
			if (sym->HasFlag(zPAR_FLAG_RETURN)) {
				switch (sym->GetOffset()) {
					case 2:
						name += "INT";
						break;
					case 1:
						name += "FLOAT";
						break;
					case 3:
						name += "STRING";
						break;
					case 7:
						name += "INSTANCE";
						break;
					default:
						break;
				}
			} else {
				name += "VOID";
			}

			name += " " + sym->name + " ( ";

			for (unsigned e = 0; e < sym->ele; ++e) {
				auto var = symtab->GetSymbol(i + e + 1);
				switch (var->GetType()) {
				case 2:
					name += "VAR INT i";
					break;
				case 1:
					name += "VAR FLOAT r";
					break;
				case 3:
					name += "VAR STRING s";
					break;
				case 5:
					name += "VAR FUNC f";
					break;
				case 7:
					name += "VAR INSTANCE n";
					break;
				default:
					break;
				}

				name += e;
				if (e < sym->ele)
					name += ", ";

			}

			name += " ) { ";
			if (sym->HasFlag(zPAR_FLAG_RETURN))
				name += "return 0; ";
			name += "};";
			name += "\n";

			zfile->Write(name);
		}
	}

	zfile->Close();
	delete zfile;
}

int zCParser::SaveGlobalVars(zCArchiver& arc)
{
	unsigned numSymbols = 0;
	for (unsigned i = 0; i < symtab.GetNumInList(); ++i ) {
		auto sym = symtab.GetSymbol(i);
		if (sym && sym->GetType() == zPAR_TYPE_INT && !sym->flags && sym->ele > 0)
			++numSymbols;
	}

	arc->WriteInt("numSymbols", numSymbols);

	for (unsigned i = 0; i < symtab.GetNumInList(); ++i ) {
		auto sym = symtab.GetSymbol(i);
		if (!sym || sym->GetType() != zPAR_TYPE_INT)
			continue;
		if (sym->flags)
			continue;
		if (sym->ele <= 0)
			continue;

		auto nam = "symName"_s + i;
		arc.WriteString(nam, sym->name);
		arc.WriteString(nam + "cnt", sym->ele);

		for (unsigned e = 0; e < sym->ele; ++i) {
			int val;
			sym->GetValue(&val, e);

			arc->WriteInt("symValue"_s + i + "_" + e, val);
		}
	}

	return 1;
}

int zCParser::LoadGlobalVars(zCArchiver& arc)
{
	int numSymbols;
	arc->ReadInt("numSymbols", &numSymbols);

	for (auto i = 0; i < numSymbols; ++i) {
		auto nam = "symName"_s + i;

		zSTRING symName;
		arc->ReadString(nam, symName);
		symName.Upper();
		auto = symtab.GetSymbol(symName);
		if (sym && sym->GetType() == zPAR_TYPE_INT && !sym->flags && sym->ele > 0) {
			auto cnt = 0;
			arc->ReadInt(nam + "cnt", cnt);
			for (auto e = 0; e < cnt; ++e) {
				int val;
				arc->ReadInt("symValue"_s + i + "_" + e, val);
				sym->SetValue(val, e);
			}
		} else {
			int val;
			arc->ReadInt("symValue"_s + i, val);
		}
	}

	return 1;
}

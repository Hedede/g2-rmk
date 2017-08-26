uint8_t tree_version = 50;

struct zCParser {
	typedef void (*MessageFunc) (zSTRING);

	static uint8_t GetVersion()
	{
		return tree_version;
	}

	static void SetVerssion(uint8_t ver)
	{
		tree_version = ver;
	}

	zCParser(int symtab_size = 100);

	int SaveDat(zSTRING& fileName);
	int LoadDat(zSTRING& fileName);

	int Error()
	{
		return error;
	}

	void SetMessageFunc(MessageFunc func)
	{
		msgfunc = func;
	}
	
	int GetLastErrorLine() const
	{
		return errorline;
	}

	int GetCurrentFunc() const
	{
		return labelcount;
	}

	int GetLastInstance() const
	{
		return instance;
	}

	void SetStopOnError(int stop)
	{
		stop_on_error = stop;
	}

	void EnableDatSave(zBOOL enable)
	{
		datsave = enable;
	}

	void EnableTreeSave(zBOOL enable)
	{
		treesave = enable;
	}

	void EnableTreeLoad(zBOOL enable)
	{
		treeload = enable;

	}

	int GetNumFiles() const
	{
		return files.GetNumInList();
	}

	zSTRING GetFileName(int idx)
	{
		if (idx > file.GetNum()) {
			return "";
		}
		return files[idx]->GetName();
	}


	bool IsStackFull() const
	{
		return stack.ReachedEnd();
	}

	void PrevWord();
	void ReadWord(zSTRING& word);

	int GetNextToken();

	void DeclareAssign(zSTRING& name);
	void DeclareAssignFunc(zSTRING& name);
	void DeclareFuncCall(zSTRING& name, int typematch);
	void DeclareReturn();
	void DeclareClass();
	void DeclareVar();
	void DeclareFunc();
	void DeclarePrototype();
	void DeclareIf();

	zCPar_TreeNode* Parse_Expression_Primary(int& tok);
	zCPar_TreeNode* Parse_Expression(int& tok, int prec);
	zCPar_TreeNode* ParseExpression()
	{
		auto tok = GetNextToken();
		return ParseExpression(tok, -1);
	}

	int ParseFile(zSTRING& fileName);
	void ParseBlock();

	int* CallFunc(int funcIndex, ...);
	int* CallFunc(zSTRING const& func)
	{
		auto index = symtab.GetIndex(func);
		return CallFunc(index);
	}

	void GetParameter(zSTRING& out);
	void GetParameter(float& out);
	void GetParameter(int& out);

	zCPar_Symbol* GetSymbol(zSTRING const& name);
	zCPar_Symbol* GetSymbol(int index);
	zCPar_Symbol* GetSymbolInfo(int index)
	{
		return GetSymbol(index);
	}
	zSTRING GetSymbolInfo(int nr, int& typ, int& ele);
	void GetClassVarInfo(int index, zSTRING& varName, int& typ, int& ele);
	zSTRING GetClassVarInfo(cindex, int idx, int& typ, int& ele);

	int GetBaseClass(zCPar_Symbol* symbol)
	int GetBaseClass(int index);
	int GetBase(int index);

	void* GetInstance();
	void* GetInstanceAndIndex(int& index);

	int GetInstance(int classId, int pos);
	int GetPrototype(int classId, int pos);

	int SaveInstance(zSTRING& name, void *adr)
	{
		name.Upper();
		auto idx = symtab.GetIndex(name);
		return SaveInstance(idx, adr);
	}

	int SetInstance(int instanceId, void* newAddr);
	int SetInstance(zSTRING const& name, void* newAddr);

	zBOOL CreatePrototype(int index, void* new_offset);
	zBOOL CreateInstance(int instanceId, void* mem);

	zBOOL CreatePrototype(zSTRING& name, void* adr)
	{
		auto idx = symtab.GetIndex(name);
		return CreatePrototype(idx, adr);
	}
	zBOOL CreateInstance(zSTRING& name, void* adr)
	{
		name.Upper();
		auto idx = symtab.GetIndex(name);
		return CreateInstance(idx, adr);
	}

	zSTRING GetText(int symidx, int idx);
	zSTRING GetText(zSTRING& name, int array);

	zSTRING GetInstanceValue(int cindex, unsigned int nr, char *adr, int array);
	zSTRING GetInstanceValue(zSTRING& name, unsigned nr, void* adr, int array);


	int FindIndex(zSTRING& varName);
	int FindInstanceVar(zSTRING& name);

	void ShowSymbols()
	{
		symtab.Show();
	}

	zBOOL MatchClass(int index, zSTRING const& name);
	void AddClassOffset(zSTRING& name, int newOffset);
	void* EvalLeaf(zCPar_TreeNode* node, int w1);
	int IsValid(zSTRING& className, void *data, zSTRING& p);

	void CreateVarReferenceList(zSTRING const& className, zCArray<int>& refList);
	int Reparse(zSTRING& fileName);
	int Parse(zSTRING fileName);

	void SaveInfoFile(zSTRING fileName);
	void SetInfoFile(zCList<zSTRING>* funcList, zSTRING const& fileName);
	int IsInAdditionalInfo(zSTRING const& name);
	int WriteAdditionalInfo(zSTRING& call, int zeile, int filepos);

	int AutoCompletion(zSTRING& str);

	int CheckClassSize(int index, int size);
	int CheckClassSize(zSTRING& className, int size)

	void DefineExternalVar(zSTRING& name, void* adr, short type, uint16_t ele);

	void ShowCode(int index);
	void ShowPCodeSpy(zSTRING& name);
	void ShowPCode(int adr, zCView *win_code, int instack);
	void CloseViews()
	{
		if ( win_code )
			screen->RemoveItem(win_code);
		debugmode = 0;
	}

	void CreatePCode();

	int SaveGlobalVars(zCArchiver& arc);
	int LoadGlobalVars(zCArchiver& arc);

	void Reset();
	int ResetGlobalVars();
	int ClearAllInstanceRefs();
	int ClearInstanceRefs(void* adr);

private:
	static bool enableParsing;

	void Error(zSTRING& errmsg, int);

	void FindNext(char* str);

	zCPar_TreeNode* CreateLeaf(char tok, zCPar_TreeNode *node);
	zCPar_TreeNode* CreateFloatLeaf();
	zCPar_TreeNode* CreateStringLeaf();
	zCPar_TreeNode* PushTree(zCPar_TreeNode *node);

	zCPar_TreeNode* ParseExpressionEx(zSTRING& tok);

	zCPar_Symbol* SearchFuncWithStartAddress(int startAddress);

	void Match(zSTRING& match);

	int ReadVarType();
	int ReadFuncType();
	int ReadArray();
	int ReadInt();
	float ReadFloat();
	void ReadString(zSTRING& out);

private:
	MessageFunc msgfunc = nullptr;

	// Liste von eingebundenen Files
	zCArray<zCPar_File*> files;

	// Symboltable
	zCPar_SymbolTable     symtab;
	zCPar_StringTable     strings;

	zCPar_Stack stack;

	zCPar_DataStack datastack;

	zCPar_Symbol* self = nullptr;


	zSTRING fname;
	zSTRING mainfile;

	zBOOL compiled = false;
	zBOOL treesave = false;
	zBOOL datsave  = true;
	zBOOL parse_changed = false;
	zBOOL treeload  = false;
	zBOOL mergemode = false;

	int linkingnr = -1;

	int linec      = 0;
	int line_start = 0;

	zBOOL ext_parse = false;

	char* pc_start   = nullptr;
	char* pc         = nullptr;
	char* oldpc      = nullptr;
	char* pc_stop    = nullptr;
	char* oldpc_stop = nullptr;

	zBOOL params = false;

	int in_funcnr   = -1;
	int in_classnr  = -1;

	int stringcount = 10000;

	zCPar_Symbol*  in_func  = nullptr;
	zCPar_Symbol*  in_class = nullptr;

	zBOOL error = false;
	zBOOL stop_on_error = true;
	int errorline = 0;

	char* prevword_index[16];
	int prevline_index[16];

	int prevword_nr = 0;
	zSTRING aword;

	int instance = -1;
	int instance_help = -1;

	zCViewProgressBar* progressBar = nullptr;

	zCPar_TreeNode* tree = nullptr;
	zCPar_TreeNode* treenode = nullptr;

	zCView* win_code = nullptr;
	zBOOL debugmode = -1;
	int curfuncnr;

	int labelcount = 0;
	int* labelpos  = nullptr;

	zCList<zSTRING> add_funclist;

	zSTRING add_filename;
	int add_created;
};
zCParser parser(1000);

/* Operatoren nehmen zwei Werte vom Datenstack und verrechnen sie.
 * Das Ergebnis schieben sie auf den Datenstack. */
enum zPAR_TOK {
	//Gewöhnliche Operatoren
	zPAR_OP_PLUS      = 0,            //"+" 0x00
	zPAR_OP_MINUS     = 1,            //"-" 0x01
	zPAR_OP_MUL       = 2,            //"*" 0x02
	zPAR_OP_DIV       = 3,            //"/" 0x03
	zPAR_OP_MOD       = 4,            //"%" 0x04
	zPAR_OP_OR        = 5,            //"|" 0x05
	zPAR_OP_AND       = 6,            //"&" 0x06
	zPAR_OP_LOWER     = 7,            //"<" 0x07
	zPAR_OP_HIGHER    = 8,            //">" 0x08

	//Ausnahme:
	zPAR_OP_IS        = 9,            //"=" 0x09 //hole i1, i2 vom Datenstack und setze i1 = i2. i1 muss hier eine Referenz sein.

	//Operatoren aus zwei Zeichen                     
	zPAR_OP_LOG_OR        = 11,       //"||"0x0B
	zPAR_OP_LOG_AND       = 12,       //"&&"0x0C
	zPAR_OP_SHIFTL        = 13,       //"<<"0x0D
	zPAR_OP_SHIFTR        = 14,       //">>"0x0E
	zPAR_OP_LOWER_EQ      = 15,       //"<="0x0F
	zPAR_OP_EQUAL         = 16,       //"=="0x10
	zPAR_OP_NOTEQUAL      = 17,       //"!="0x11
	zPAR_OP_HIGHER_EQ     = 18,       //">="0x12
	zPAR_OP_ISPLUS        = 19,       //"+="0x13
	zPAR_OP_ISMINUS       = 20,       //"-="0x14
	zPAR_OP_ISMUL         = 21,       //"*="0x15
	zPAR_OP_ISDIV         = 22,       //"/="0x16

	/* Unäre Operatoren nehmen natürlich nur einen Wert vom Datenstack.
	   Sie schieben ebenfalls das Ergebnis auf den Datenstack. */
	zPAR_OP_UNARY         = 30,
	zPAR_OP_UN_PLUS       = 30,       //"+" 0x1E
	zPAR_OP_UN_MINUS      = 31,       //"-" 0x1F
	zPAR_OP_UN_NOT        = 32,       //"!" 0x20
	zPAR_OP_UN_NEG        = 33,       //"~" 0x21
	zPAR_OP_MAX           = 33,

	//Jetzt: Andere Tokens (zum Parsen)
	zPAR_TOK_BRACKETON    = 40,
	zPAR_TOK_BRACKETOFF   = 41,
	zPAR_TOK_SEMIKOLON    = 42,
	zPAR_TOK_KOMMA        = 43,
	zPAR_TOK_SCHWEIF      = 44,
	zPAR_TOK_NONE         = 45,

	zPAR_TOK_FLOAT        = 51,
	zPAR_TOK_VAR          = 52,
	zPAR_TOK_OPERATOR     = 53,

	/* Weitere Befehle (Nicht vergessen: Operatoren sind auch Befehle!)
	 * Manchen Befehlen folgt immer ein Parameter nach.
	 * Anderen Befehle bestehen nur aus dem Befehlstoken */
	zPAR_TOK_RET          = 60, //0x3C    //return
	zPAR_TOK_CALL         = 61, //0x3D    //rufe Funktion auf. Es folgen 4 byte Callziel (Stackadresse)
	zPAR_TOK_CALLEXTERN   = 62, //0x3E    //rufe External auf. Es folgen 4 byte Symbol der External
	zPAR_TOK_POPINT       = 63, //0x3F    //ungenutzt
	zPAR_TOK_PUSHINT      = 64, //0x40    //schiebe Integer Konstante auf den Datenstack. Es folgt die Integerkonstante (4 byte)
	zPAR_TOK_PUSHVAR      = 65, //0x41    //schiebe Variable auf den Datenstack. Es folt der Symbolindex der Variable (4 byte)
	zPAR_TOK_PUSHSTR      = 66, //0x42    //ungenutzt
	zPAR_TOK_PUSHINST     = 67, //0x43    //schiebe Instanz auf den Datenstack. Es folgen die 4 byte Symbolindex der Instanz
	zPAR_TOK_PUSHINDEX    = 68, //0x44    //ungenutzt
	zPAR_TOK_POPVAR       = 69, //0x45    //ungenutzt
	zPAR_TOK_ASSIGNSTR    = 70, //0x46    //Zuweisung. Hole v1, v2 vom Stack und setze v1 = v2
	zPAR_TOK_ASSIGNSTRP   = 71, //0x47    //Zuweisung. Hole v1, v2 vom Stack und setze v1 = v2
	zPAR_TOK_ASSIGNFUNC   = 72, //0x48    //Zuweisung. Hole v1, v2 vom Stack und setze v1 = v2
	zPAR_TOK_ASSIGNFLOAT  = 73, //0x49    //Zuweisung. Hole v1, v2 vom Stack und setze v1 = v2
	zPAR_TOK_ASSIGNINST   = 74, //0x4A    //Zuweisung. Hole v1, v2 vom Stack und setze v1 = v2
	zPAR_TOK_JUMP         = 75, //0x4B    //Springe im Parserstack. Es folgen 4 byte Sprungziel
	zPAR_TOK_JUMPF        = 76, //0x4C    //Hole b vom Datenstack und springe, falls b == 0 ist. Es folgen 4 byte Sprungziel (lies: "jump if false". Für "if"-Bedingungen)

	zPAR_TOK_SETINSTANCE  = 80, //0x50    //Es folgen 4 byte Symbolindex. Umständlich zu erklären. So grob: "Setze aktuelle Instanz."

	//Irgendwelche Internas. Wahrscheinlich nur bedingt relevant.
	zPAR_TOK_SKIP         = 90,
	zPAR_TOK_LABEL        = 91,
	zPAR_TOK_FUNC         = 92,
	zPAR_TOK_FUNCEND      = 93,
	zPAR_TOK_CLASS        = 94,
	zPAR_TOK_CLASSEND     = 95,
	zPAR_TOK_INSTANCE     = 96,
	zPAR_TOK_INSTANCEEND  = 97,
	zPAR_TOK_NEWSTRING    = 98,

	//Für Pushen von Arraywerten.
	zPAR_TOK_FLAGARRAY      = zPAR_TOK_VAR | 128, //nicht nutzen!
	zPAR_TOK_PUSH_ARRAYVAR  = zPAR_TOK_FLAGARRAY + zPAR_TOK_PUSHVAR, //Hole 4 Byte Symbolindex vom Stack. Hole 1 Byte Arrayindex vom Stack. Schiebe Adresse des entsprechenden Arrayelements im Symbol auf den Stack.
};

/* Siehe auch diesen Beitrag:
 * http://forum.worldofplayers.de/forum/showthread.php?p=13086904&#post13086904
 * indem ich vorschlage, wie man den Parser verstehen lernen könnte. */

const zSTRING PARSER_TOKEN_NAMES[246] = {
	"zPAR_OP_PLUS          ",
	"zPAR_OP_MINUS         ",
	"zPAR_OP_MUL           ",
	"zPAR_OP_DIV           ",
	"zPAR_OP_MOD           ",
	"zPAR_OP_OR            ",
	"zPAR_OP_AND           ",
	"zPAR_OP_LOWER         ",
	"zPAR_OP_HIGHER        ",
	"zPAR_OP_IS            ",
	"[INVALID_TOKEN]       ",
	"zPAR_OP_LOG_OR        ",
	"zPAR_OP_LOG_AND       ",
	"zPAR_OP_SHIFTL        ",
	"zPAR_OP_SHIFTR        ",
	"zPAR_OP_LOWER_EQ      ",
	"zPAR_OP_EQUAL         ",
	"zPAR_OP_NOTEQUAL      ",
	"zPAR_OP_HIGHER_EQ     ",
	"zPAR_OP_ISPLUS        ",
	"zPAR_OP_ISMINUS       ",
	"zPAR_OP_ISMUL         ",
	"zPAR_OP_ISDIV         ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_OP_UNARY         ",
	"zPAR_OP_UN_PLUS       ",
	"zPAR_OP_UN_MINUS      ",
	"zPAR_OP_UN_NOT        ",
	"zPAR_OP_UN_NEG        ",
	"zPAR_OP_MAX           ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_BRACKETON    ",
	"zPAR_TOK_BRACKETOFF   ",
	"zPAR_TOK_SEMIKOLON    ",
	"zPAR_TOK_KOMMA        ",
	"zPAR_TOK_SCHWEIF      ",
	"zPAR_TOK_NONE         ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_FLOAT        ",
	"zPAR_TOK_VAR          ",
	"zPAR_TOK_OPERATOR     ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_RET          ",
	"zPAR_TOK_CALL         ",
	"zPAR_TOK_CALLEXTERN   ",
	"zPAR_TOK_POPINT       ",
	"zPAR_TOK_PUSHINT      ",
	"zPAR_TOK_PUSHVAR      ",
	"zPAR_TOK_PUSHSTR      ",
	"zPAR_TOK_PUSHINST     ",
	"zPAR_TOK_PUSHINDEX    ",
	"zPAR_TOK_POPVAR       ",
	"zPAR_TOK_ASSIGNSTR    ",
	"zPAR_TOK_ASSIGNSTRP   ",
	"zPAR_TOK_ASSIGNFUNC   ",
	"zPAR_TOK_ASSIGNFLOAT  ",
	"zPAR_TOK_ASSIGNINST   ",
	"zPAR_TOK_JUMP         ",
	"zPAR_TOK_JUMPF        ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_SETINSTANCE  ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_SKIP         ",
	"zPAR_TOK_LABEL        ",
	"zPAR_TOK_FUNC         ",
	"zPAR_TOK_FUNCEND      ",
	"zPAR_TOK_CLASS        ",
	"zPAR_TOK_CLASSEND     ",
	"zPAR_TOK_INSTANCE     ",
	"zPAR_TOK_INSTANCEEND  ",
	"zPAR_TOK_NEWSTRING    ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_FLAGARRAY    ",  
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"[INVALID_TOKEN]       ",
	"zPAR_TOK_PUSH_ARRAYVAR"
};

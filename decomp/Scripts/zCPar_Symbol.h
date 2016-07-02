enum zPAR_FLAGS : uint16_t {
	zPAR_FLAG_CONST       = 1,
	zPAR_FLAG_RETURN      = 2,
	zPAR_FLAG_CLASSVAR    = 4,
	zPAR_FLAG_EXTERNAL    = 8,
	zPAR_FLAG_MERGED      = 16,
	zPAR_FLAG_ALLOC_SPACE = 64,
};

enum zPAR_TYPE {
	zPAR_TYPE_VOID,
	zPAR_TYPE_FLOAT,
	zPAR_TYPE_INT,
	zPAR_TYPE_STRING,
	zPAR_TYPE_CLASS,
	zPAR_TYPE_FUNC,
	zPAR_TYPE_PROTOTYPE,
	zPAR_TYPE_INSTANCE
};

struct zCPar_Symbol {
	zCPar_Symbol() = default;

	zPAR_TYPE GetType() const
	{
		return type;
	}

private:
	zSTRING name;
	int next = 0;

	//Inhalt des Symbols, Pointer oder Primitiver Typ
	union {
		void*    data_ptr;
		int*     data_pint;
		float*   data_pfloat;
		zSTRING* data_pstring;
		int      data_int;
		float    data_float;
	}; // content;

	// Offset bei Klassenvariablen
	// Adresse bei Instanzen
	// Rückgabewert bei Funktionen
	int offset = 0;

	uint16_t ele  : 12;
	uint16_t type : 4;
	zPAR_FLAGS flags  = 0;

	int filenr   = 0;
	int line     = 0;
	int line_anz = 0;
	int pos_beg  = 0;
	int pos_anz  = 0;

	zCPar_Symbol* parent = nullptr;
};

class zCPar_SymbolTable {
	int GetNumInList()
	{
		return table.GetNumInList();
	}

	zCPar_SymbolTable* GetLastSymbol()
	{
		return lastsym;
	}

	zCPar_SymbolTable* GetFirstSymbol()
	{
		return firstsym;
	}

	void ShrinkToFit()
	{
		table.ShrinkToFit();
		tablesort.ShrinkToFit();
	}

private:
	zCArray<zCPar_Symbol*> table;
	zCArraySort<int> tablesort;
	zCPar_Symbol*  lastsym;
	zCPar_Symbol*  firstsym;

}

struct zCPar_StringTable {
	zCPar_StringTable(int num)
		: table(num)
	{ }

	~zCPar_StringTable()
	{
		Clear();
	}

	int GetNumInList()
	{
		return table.GetNumInList();
	}

	int GetLastEntry()
	{
		return GetNumInList() - 1;
	}

	zSTRING* GetString(int idx)
	{
		if (idx > GetNumInList())
			return nullptr;
		return table[idx];
	}

	void Clear()
	{
		for (auto string : table)
			Delete(string);
		table.DeleteList();
	}

	void ShrinkToFit()
	{
		table.ShrinkToFit();
	}

	void Show()
	{
		for (auto string : table) {
			if (string)
				printf("%s\n", string.Data());
		}
	}

	int Save(zSTRING& fileName)
	{
		auto file = zfactory->CreateZFile(fileName);
		file->Create();

		for (auto string : table) {
			auto str = string ? *string : "";
			file->Write(str);
			file->Write("\n");
		}
		file->Close();
		delete file;
	}

	void Insert(zSTRING& s)
	{
		table.Insert(new zSTRING{s});
	}

private:
	zCArray<zSTRING*> table;
};

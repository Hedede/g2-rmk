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

class zCPar_Symbol {
public:
	zCPar_Symbol() = default;
private:
	zSTRING name;
	int next = 0;

	//Inhalt des Symbols, Pointer oder Primitiver Typ
	union {
		void *data_ptr;
		int *data_pint;
		float *data_pfloat;
		zSTRING *data_pstring;
		int data_int;
		float data_float;
	} content;

	// Offset bei Klassenvariablen
	// Adresse bei Instanzen
	// Rückgabewert bei Funktionen
	int offset = 0;

	static const int bitfield_ele         = ((1 << 12) - 1) <<  0;
	static const int bitfield_type        = ((1 <<  4) - 1) << 12;

	uint16_t bitfield = 0;
	zPAR_FLAGS flags  = 0;

	int filenr   = 0;
	int line     = 0;
	int line_anz = 0;
	int pos_beg  = 0;
	int pos_anz  = 0;

	zCPar_Symbol* parent = nullptr;
};

class zCPar_SymbolTable {
public:
	int GetNumInList()
	{
		return this->table.size();
	}

	zCPar_SymbolTable* GetLastSymbol()
	{
		return lastsym;
	}

	zCPar_SymbolTable* GetFirstSymbol()
	{
		return firstsym;
	}
private:
	zCArray<zCPar_Symbol*> table;
	zCArraySort<int> tablesort;
	zCPar_Symbol*  lastsym;
	zCPar_Symbol*  firstsym;

}

class zCPar_StringTable {
	zCArray <zSTRING *> array;
};

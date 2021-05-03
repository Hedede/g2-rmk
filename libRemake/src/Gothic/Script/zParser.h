#ifndef Gothic_zParser_H
#define Gothic_zParser_H
#include <vector>
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Gothic/Types/Base.h>
#include <Gothic/Types/zSTRING.h>
#include <aw/types/support/reinterpret.h>

enum zPAR_TYPE
{
	zPAR_TYPE_VOID      = 0,
	zPAR_TYPE_FLOAT     = 1,
	zPAR_TYPE_INT       = 2,
	zPAR_TYPE_STRING    = 3,
	zPAR_TYPE_CLASS     = 4,
	zPAR_TYPE_FUNC      = 5,
	zPAR_TYPE_PROTOTYPE = 6,
	zPAR_TYPE_INSTANCE  = 7,
};

enum zPAR_FLAG : uint16_t
{
	zPAR_FLAG_CONST       = 0x1,
	zPAR_FLAG_RETURN      = 0x2,
	zPAR_FLAG_CLASSVAR    = 0x4,
	zPAR_FLAG_EXTERNAL    = 0x8,
	zPAR_FLAG_MERGED      = 0x10,
	zPAR_FLAG_ALLOC_SAPCE = 0x40,
};

inline zPAR_FLAG operator|(zPAR_FLAG a, zPAR_FLAG b)
{
	return zPAR_FLAG(uint16_t(a)|uint16_t(b));
}

struct zCPar_File;
struct zCPar_Symbol {
	template<typename T>
	T GetValue(int index = 0) const;

	template<typename T>
	void SetValue(T value, int index = 0);

	std::string GetName() const
	{
		return std::string(name);
	}

	void SetName(std::string const& name)
	{
		this->name = name;
		this->name.Upper();
	}

	void SetType(zPAR_TYPE type)
	{
		this->type = static_cast<uint16_t>(type);
	}

	void SetFlag(zPAR_FLAG flag)
	{
		this->flags |= static_cast<uint16_t>(flag);
	}

	void SetOffset(int offset)
	{
		this->offset = offset;
	}

	void SetStackPos(int pos)
	{
		data_int = pos;
	}

	void SetParent(zCPar_Symbol* parent)
	{
		this->parent = parent;
	}

	std::string DebugPrintValue(int index) const;
	std::string DebugPrint() const;

	zSTRING name;

	zCPar_Symbol *next;

	union {
		void*    data_ptr;
		int*     data_pint;
		float*   data_pfloat;
		zSTRING* data_pstring;
		int      data_int;
		float    data_float;
	};

	int offset;

	uint16_t ele  : 12;
	uint16_t type : 4;

	uint16_t flags;

	int filenr;
	int line;
	int line_anz;
	int pos_beg;
	int pos_anz;

	zCPar_Symbol *parent;
};

template<>
inline float zCPar_Symbol::GetValue(int index) const
{
	if (ele <= 1)
		return data_float;
	if (index >= ele)
		return 0.0f;
	return data_pfloat[index];
}

template<>
inline int zCPar_Symbol::GetValue(int index) const
{
	if (ele <= 1)
		return data_int;
	if (index >= ele)
		return 0;
	return data_pint[index];
}

template<>
inline zSTRING zCPar_Symbol::GetValue(int index) const
{
	if (index >= ele)
		return "";
	return data_pstring[index];
}

template<>
inline std::string zCPar_Symbol::GetValue(int index) const
{
	if (index >= ele)
		return {};
	return static_cast<std::string>(data_pstring[index]);
}

template<>
inline void zCPar_Symbol::SetValue(float value, int index)
{
	if (ele <= 1)
		data_float = value;
	else if (index < ele)
		data_pfloat[index] = value;
}

template<>
inline void zCPar_Symbol::SetValue(int value, int index)
{
	if (ele <= 1)
		data_int = value;
	else if (index < ele)
		data_pint[index] = value;
}

template<>
inline void zCPar_Symbol::SetValue(zSTRING value, int index)
{
	if (index < ele)
		data_pstring[index] = value;
}

template<>
inline void zCPar_Symbol::SetValue(std::string value, int index)
{
	if (index < ele)
		data_pstring[index] = value;
}


struct zCPar_TreeNode;

struct zCPar_Stack {
	char *stack;
	char *stackptr;
	char *stacklast;
	int stacksize;
};

struct zCPar_StringTable
{
	zCArray<zSTRING*> table;
};


struct zCPar_DataStack {
	int stack[2048];
	int sptr;
};

struct zCPar_SymbolTable {
	bool Insert(zCPar_Symbol* symbol)
	{
		Thiscall<int(zCPar_SymbolTable*,zCPar_Symbol*)> call{0x7A3F00};
		return call(this, symbol);
	}

	zCPar_Symbol* GetSymbol(int index)
	{
		if ( index < 0 || index >= table.numInArray )
			return nullptr;

		return table[index];
	}

	zCPar_Symbol *preAllocatedSymbols;
	int nextPreAllocated;
	zCArray<zCPar_Symbol*> table;
	zCArraySort<int> tablesort;
	zCPar_Symbol *lastsym;
	zCPar_Symbol *firstsym;
};

struct zCViewProgressBar;
struct zCView;

struct zCParser {
	static zBOOL& enableParsing;

	using MessageFunc = void (*)(zSTRING);

	zCParser(size_t symtab_size = 100)
	{
		Thiscall<void(zCParser*,int)> call{0x78DED0};
		call(this, symtab_size);
	}


	bool Parse(std::string const& fileName)
	{
		Thiscall<int(zCParser*, zSTRING)> call{0x78EBA0};
		return call(this, fileName);
	}

	void CreatePCode()
	{
		Thiscall<void(zCParser*)> call{0x7900E0};
		call(this);
	}

	int PopDataValue()
	{
		Thiscall<int(zCParser*)> call{0x7918B0};
		return call(this);
	}

	std::string PopString()
	{
		Thiscall<zSTRING*(zCParser*)> call{0x791940};
		zSTRING* str = call(this);
		if (str)
			return std::string(*str);
		return "-NOSTRING-";
	}

	void Reset()
	{
		Thiscall<void(zCParser*)> call{0x793100};
		call(this);
	}

	bool CheckClassSize(std::string const& className, size_t size)
	{
		Thiscall<int(zCParser*, zSTRING const&, int)> call{0x794450};
		return call(this, className, size);
	}

	void AddClassOffset(std::string const& name, int newOffset)
	{
		zSTRING zName = name;
		Thiscall<void(zCParser*, zSTRING&, int)> call{0x794730};
		call(this, zName, newOffset);
	}

	int GetIndex(std::string_view name)
	{
		const zSTRING tmpstring = name;
		return thiscall<int>(0x793470, this, &tmpstring);
	}

	zCPar_Symbol* GetSymbol(std::string const& name)
	{
		const zSTRING tmpstring = name;
		return thiscall<zCPar_Symbol*>(0x7938D0, this, &tmpstring);
	}

	zCPar_Symbol* GetSymbol(int index)
	{
		return symtab.GetSymbol(index);
	}

	bool CreateInstance(std::string const& name, void* adr)
	{
		Thiscall<int(zCParser*, zSTRING const&, void*)> call{0x792F20};
		return call( this, name, adr );
	}

	template<typename...Args>
	FORCE_INLINE void CallFunc(int index, Args...args)
	{
		call(0x7929F0, this, index, args...);
	}



	template<typename...Args>
	void DefineExternal(std::string const& funcName, int (*func)(), zPAR_TYPE type, Args...args)
	{
		std::vector<zPAR_TYPE> argList{ args ... };
		return DefineExternal( funcName, func, type, argList );
	}

	void DefineExternal(std::string const& name, int (*func)(), zPAR_TYPE ret, std::vector<zPAR_TYPE> args);

	void GetParameter(int& value)
	{
		value = PopDataValue();
	}

	void GetParameter(float& value)
	{
		value = PopDataValue();
	}

	void GetParameter(std::string& value)
	{
		//g2::Log("PARSER","Stack ptr: ", datastack.sptr);
		value = PopString();
	}

	void EnableTreeSave(bool enable)
	{
		treesave = enable;
	}

	void EnableTreeLoad(bool enable)
	{
		treeload = enable;
	}

	bool Error() const
	{
		return error;
	}

	void SetProgressBar(zCViewProgressBar* pb)
	{
		progressBar = pb;
	}

	void SetStopOnError(bool stop)
	{
		stop_on_error = stop;
	}

private:
	MessageFunc msgfunc = nullptr;

	// Liste von eingebundenen Files
	zCArray<zCPar_File*> files;

	// Symboltable
	zCPar_SymbolTable  symtab;
	zCPar_StringTable  strings;

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

	int labelcount = 0;
	int* labelpos  = nullptr;

	zCList<zSTRING> add_funclist;

	zSTRING add_filename;
	int add_created;
};


inline auto& zparser = Value<zCParser>(0xAB40C0);
#endif//Gothic_zParser_H

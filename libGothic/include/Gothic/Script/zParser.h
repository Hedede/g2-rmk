#ifndef Gothic_zParser_H
#define Gothic_zParser_H
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Gothic/Types/Base.h>
#include <Gothic/Types/zSTRING.h>


struct zCPar_File;
struct zCPar_Symbol {
	template<typename T>
	T GetValue(int index);
	
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
inline float zCPar_Symbol::GetValue(int index)
{
	if (ele <= 1)
		return data_float;
	return data_pfloat[index];
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

	zCParser(size_t symtab_size)
	{
		Thiscall<void(zCParser*,int)> call{0x78DED0};
		call(this, symtab_size);
	}

	zCPar_Symbol* GetSymbol(std::string const& name)
	{
		Thiscall<zCPar_Symbol*(zCParser*, zSTRING const&)> call{0x7938D0};
		return call(this, name);
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

#include <Hook/size_checker.h>
namespace { size_checker<zCParser, 0x21C4>  sc_zparsize; }

inline auto& zparser = Value<zCParser>(0xAB40C0);
#endif//Gothic_zParser_H

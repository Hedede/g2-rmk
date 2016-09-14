#ifndef Gothic_zParser_H
#define Gothic_zParser_H
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>
#include <Gothic/Types/zCList.h>
#include <Gothic/Types/Base.h>
#include <Gothic/Types/zSTRING.h>


struct zCPar_File;
struct zCPar_Symbol;
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

#endif//Gothic_zParser_H

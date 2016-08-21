void zCPar_SymbolTable::Clear()
{
	if ( preAllocatedSymbols )
	{
		delete[] preAllocatedSymbols;
		preAllocatedSymbols = 0;
		nextPreAllocated = -1;
	}
	else
	{
		for (int i, num = table.NumInList; i < num; ++i) {
			if (table[i])
				delete table[i];
		}
	}

	table.Clear();
	tablesort.Clear();
	firstsym = 0;
	lastsym = 0;
}

int zCPar_SymbolTable::Compare(int sym1, int sym2)
{
	zCArray table = cur_table->table;

	return table[sym1].compare(table[sym2]);
}

zCPar_SymbolTable::zCPar_SymbolTable(int size)
{
	SetSize(size);
	tablesort.Compare = zCPar_SymbolTable::Compare;
	preAllocatedSymbols = 0;
	nextPreAllocated = -1;
}

zCPar_SymbolTable::~zCPar_SymbolTable()
{
	Clear(this);
}

zCPar_Symbol* zCPar_SymbolTable::PreAlloc(int count)
{
	zCPar_Symbol* newalloc =  new zCPar_Symbol[count];
	preAllocatedSymbols = newalloc;
	nextPreAllocated = count - 1;
	return preAllocatedSymbols;
}

int zCPar_SymbolTable::Search(zSTRING const& name, int begin, int end)
{
	cur_table = this;
	return tablesort.Search(name, begin, end);
}

int zCPar_SymbolTable::GetIndex(zSTRING const& name)
{
	return Search(name, 0, table.size() - 1);
}

int zCPar_SymbolTable::GetIndex(zSTRING const& name, int index)
{
	zCPar_Symbol* sym;
	if ( max < 0 || max >= table.size() )
		sym = 0;
	else
		sym = table[max];

	int begin = index + 1;
	int end = (sym->bitfield & 0xFFF) + index + 1;

	return Search(name, begin, end);
}

int zCPar_SymbolTable::GetIndex(zCPar_Symbol* symbol)
{
	return GetIndex(symbol->name);
}

zCPar_Symbol * zCPar_SymbolTable::GetSymbol(int index)
{
	if (index < 0 || index >= table.size())
		return 0;

	return table[index];
}

zCPar_Symbol * zCPar_SymbolTable::GetSymbol(zSTRING const& name)
{
	int index = Search(0, table.size());
	return GetSymbol(index);
}

bool zCPar_SymbolTable::IsIn(zSTRING const& name)
{
	int index = GetIndex(name);
	return index >= 0;
}

bool zCPar_SymbolTable::IsIn(zCPar_Symbol* symbol)
{
	return IsIn(symbol->name);
}


void zCPar_SymbolTable::Load(zFILE& file)
{
	zList<zCParserExternals*> externals_list;

	for (int i = 0; i < table.numInArray; ++i) {
		zCPar_Symbol* symbol = table[i];

		if ((symbol->bitfield >> 16) & 8) {
			int val = (symbol->bitfield & 0xF000) == 0x5000 ?
				symbol->content.data_int :
				symbol.GetDataAdr(0);

			auto external = new zCParserExternals(symbol->name, val);
			if (!externals_list.IsIn(external)) {
				if (!externals_list.root)
					externals_list.last = external;

				external->next = externals_list.root;
				externals_list.root = external;
				++externals_list.count;
			}
		}

		if ( !preAllocatedSymbols )
			delete symbol;
	}

	if ( preAllocatedSymbols ) {
		delete[] preAllocatedSymbols; // &[-1]?
		preAllocatedSymbols = 0;
		nextPreAllocated = -1;
	}

	int symbol_count;
	file.Read(&symbol_count, 4);

	table.numInArray = 0;
	tablesort._numInArray = 0;
	if ( tablesort.numInArray < symbol_count )
		tablesort.AllocDelta(symbol_count - tablesort.numAlloc);

	if ( table.numInArray < symbol_count )
		table.AllocDelta(symbol_count - table.numAlloc);

	tablesort.numInArray = symbol_count; //look up later
	file.Read(tablesort.array, 4*symbol_count);


	PreAlloc(symbol_count);
	
	for (int i = 0; i < symbol_count; ++i) {
		auto* symbol = preAllocatedSymbols[nextPreAllocated--];
		prealloc->LoadFull(file);

		int parent_index;
		file.Read(&parent_index, 4);
		if (parent_index >= 0) {
			zCPar_Symbol* parent = 0;
			if (parent_index >= table.numInArray)
				parent = 0;
			
			prealloc->parent = parent;
		}

		table[table.num] = symbol;
		++table.numInArray;
	}

	zCPar_SymbolTable::cur_table = this;

	for (auto ext = externals_list.root; ext; ext = ext.next) {
		auto* symbol = GetSymbol(ext->name);
		if (symbol && (symbol.bitfield >> 16) & 8) {
			symbol->content.data_ptr = ext->ptr;
		} else {
			ZSTRING msg = "U: PAR: External " + ext->name + " not found/";
			zERROR::Warning(msg);
		}
	}

	for (auto ext = externals_list.root; ext; ext = ext.next)
		delete ext;
}

void zCPar_SymbolTable::Save(zFILE& file)
{
	int num = table.size();
	file.Write(&num, 4);
	if ( num <= 0 )
		return;

	file.Write((int *)tablesort.array, 4 * num);
	for (int idx = 0; idx < num; ++idx) {
		zCPar_Symbol *sym = table[idx];
		zCPar_Symbol* parent = sym->parent;
		sym->SaveFull(file);

		int parentId;
		if (!parent)
			parentId = -1;
		else 
			parentId = GetIndex(parent);

		file.Write(&parent, 4);
	}
}

void zCPar_SymbolTable::SetSize(int size)
{
	table.AllocAbs(size);
	tablesort.AllocAbs(size);
}

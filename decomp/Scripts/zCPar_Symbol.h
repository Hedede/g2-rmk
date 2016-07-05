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


zCPar_Symbol::~zCPar_Symbol()
{
	if (!content.data_ptr)
		return;

	if (flags & zPAR_FLAG_CLASSVAR)
		return;
	
	if (flags & zPAR_FLAG_ALLOC_SPACE)
	{
		if ((type == zPAR_TYPE_FLOAT) || (type == zPAR_TYPE_INT)) 
		{
			if (size > 1)
				delete[] (int *)content.data_ptr;
		}
		else if (type == zPAR_TYPE_STRING)
		{
			if (size <= 1)
				delete   content.data_pstring;
			else
				delete[] content.data_pstring;
		}
	}
}

void zCPar_Symbol::SetFlag(int flag)
{
	flags &= 0xFFC0 | (flag & 0x3F | flags & 0x3F);
}

bool zCPar_Symbol::HasFlag(int flag)
{
	return (flags & flag & 0x3Fu) > 0;
}

void  zCPar_Symbol::SetDataAdr(void *adr)
{
	content.data_ptr = adr;
}

void zCPar_Symbol::SetValue(zSTRING& val, int index)
{
	content.data_pstring[index] = val;
}

void zCPar_Symbol::SetValue(float val, int index)
{
	if (ele <= 1)
		content.data_float = val;
	else
		content.data_pfloat[index] = val;
}

void zCPar_Symbol::SetValue(int val, int index)
{
	if (ele <= 1)
		content.data_int = val;
	else
		content.data_pint[index] = val;
}

void  zCPar_Symbol::GetValue(int& out, int index)
{
	if (ele <= 1 )
		out = content.data_int;
	else
		out = content.data_pint[index];
}

void zCPar_Symbol::GetValue(float& out, int index)
{
	if (ele <= 1 )
		out = content.data_float;
	else
		out = content.data_pfloat[index];
}

void zCPar_Symbol::GetValue(zSTRING& out, int index)
{
	out = content.data_pstring[index];
}


void zCPar_Symbol::SetOffset(int new_offset)
{
	offset = new_offset;
}

int zCPar_Symbol::GetOffset()
{
	return offset;
}

void zCPar_Symbol::SetClassOffset(int offset)
{
	content.data_int = offset;
}

int zCPar_Symbol::GetClassOffset()
{
	return content.data_int;
}

zCPar_Symbol * zCPar_Symbol::GetNext()
{
	return next;
}

bool zCPar_Symbol::ArrayCheck(int index)
{
	return index >= 0 && index < ele;
}

void zCPar_Symbol::SetParent(zCPar_Symbol& newParent)
{
	parent = &newParent;
}

zCPar_Symbol* zCPar_Symbol::GetParent()
{
	return parent;
}

void zCPar_Symbol::SetStackPos(int pos, int)
{
	content.data_int = pos;
}

void zCPar_Symbol::GetStackPos(int *out, int)
{
	*out = content.data_int;
}

void zCPar_Symbol::SetLineData(int l1, int l2, int lpos1, int lpos2)
{
	assert(0<= l1 && l1 < (1 << 19));
	assert(0<= l2 && l2 < (1 << 19));
	assert(0 <= lpos1 && lpos1 < (1 << 24));
	assert(0 <= lpos2 && lpos2 < (1 << 24));

	line     = l1 & 0x7FFFF;
	line_anz = l2 & 0x7FFFF;

	pos_beg = lpos1 & 0xFFFFFF;
	pos_anz = lpos2 & 0xFFFFFF;
}

void zCPar_Symbol::GetLineData(int& l1, int& l2, int& lpos1, int& lpos2)
{
	l1    = line     & 0x7FFFF;
	l2    = line_anz & 0x7FFFF;
	lpos1 = pos_beg & 0xFFFFFF;
	lpos2 = pos_anz & 0xFFFFFF;
}

void zCPar_Symbol::SetFileNr(int nr)
{
	assert(nr < (1<<19));

	filenr = nr & 0x7FFFF;
}

static zCPar_Symbol* zCPar_Symbol::instance_sym;

void zCPar_Symbol::SetUseInstanceAdr(zCPar_Symbol* adr)
{
	zCPar_Symbol::instance_adr = adr;
}

void zCPar_Symbol::SetUseInstance(zCPar_Symbol* inst)
{
	if ( !inst ) {
		zCPar_Symbol::instance_adr = 0;
		zCPar_Symbol::instance_sym = 0;
		return;
	}

	zPAR_TYPE type = (inst->bitfield >> 12) & 0xF;

	if ( type == zPAR_TYPE_INSTANCE || type == zPAR_TYPE_PROTOTYPE ) {
		if ( inst->offset ) {
			zCPar_Symbol::instance_adr = (void *)inst->offset;
			zCPar_Symbol::instance_sym = inst;
		} else {
			zCPar_Symbol::instance_adr = 0;
			zCPar_Symbol::instance_sym = 0;
		}
	}
	else
	{
		zERROR::Warning("U:PAR:Object is not an instance :");
		zCPar_Symbol::instance_adr = 0;
		zCPar_Symbol::instance_sym = 0;
	}
}

zCPar_Symbol * zCPar_Symbol::GetUseInstance()
{
	return zCPar_Symbol::instance_sym;
}

void* zCPar_Symbol::GetInstanceAdr()
{
	return offset;
}

void* zCPar_Symbol::GetDataAdr(int idx)
{
	static float   dummyflt = 0.0;
	static int     dummyint = 0;
	static zSTRING dummystr = "";

	if ( flags & zPAR_FLAG_CLASSVAR ) {
		if ( zCPar_Symbol::instance_adr ) {
			char* ptr = (char *)zCPar_Symbol::instance_adr + offset;
			if ( type == zPAR_TYPE_STRING )
				return ((zSTRING*)ptr)[idx];
			return ((int*)ptr)[idx];
		}

		zSTRING msg;

		if ( zCPar_Symbol::instance_sym )
			msg = zCPar_Symbol::instance_sym->name + ": ";
		msg += this->name;

		zINFO(2,"U:PAR: Adressing an empty Instance : " + msg); //zParser_Symbol.cpp, 365

		if (type == zPAR_TYPE_STRING) {
			dummystr = "";
			return &dummystr;
		} else if (type == zPAR_TYPE_FLOAT) {
			dummyflt = 0.0;
			return &dummyflt;
		}

		dummyint = 0;
		return &dummyint;
	}

	switch ( type ) {
	case zPAR_TYPE_STRING:
		return &content.data_pstring[idx];
	case zPAR_TYPE_INT:
		if ( size > 1 )
			return &content.data_pint[idx];
	case zPAR_TYPE_FLOAT:
		if ( size > 1 )
			return &content.data_pfloat[idx];
	case zPAR_TYPE_FUNC:
	case zPAR_TYPE_PROTOTYPE:
	case zPAR_TYPE_INSTANCE:
		break;
	default:
		return content.data_ptr;
	}

	return &content;
}

void zCPar_Symbol::AllocSpace()
{
	if ( flags & zPAR_FLAG_ALLOC_SPACE )
		return;

	int size = (bitfield & 0xFFF);
	if ( flags & zPAR_FLAG_CLASSVAR ) {
		if (!size)
			zERROR::Fault("U: SYM: Allocspace with no elements !");

		int space = 0;
		switch ( type ) {
		case zPAR_TYPE_STRING:
			space = sizeof(zSTRING) * size;
			break;
		case zPAR_TYPE_FLOAT:
		case zPAR_TYPE_INT:
		case zPAR_TYPE_FUNC:
			space = 4 * size;
			break;
		default:
			break;
		}

		if ( parent ) {
			offset = parent->offset;
			parent->offset += space;
		}
	}
	else
	{
		switch ( type ) {
		case zPAR_TYPE_FLOAT:
		case zPAR_TYPE_INT:
			if ( size > 1 ) {
				content.data_ptr = operator new[](4 * size);
				break;
			}
		case zPAR_TYPE_FUNC:
		case zPAR_TYPE_PROTOTYPE:
		case zPAR_TYPE_INSTANCE:
			content.data_int = 0;
			break;
		case zPAR_TYPE_STRING:
			if (size == 1)
			{
				content.data_pstring = new zSTRING;
			}
			else
			{
				content.data_pstring = new zSTRING[size];
			}
			break;
		case zPAR_TYPE_CLASS:
			this->offset = 0;
			break;
		default:
			break;
		}
	}

	flags |= zPAR_FLAG2_ALLOC_SPACE;
}

void zCPar_Symbol::LoadFull(zFILE *file)
{
	int tmp;
	file->Read(file, &tmp, 4);

	if ( tmp )
		file->Read(this->name);

	file->Read(&this->offset, 28);
	this->next = 0;

	int type = (bitfield & bitfield_type) >> 12;
	int size =  bitfield & bitfield_ele;

	if (type != zPAR_TYPE_CLASS)
		AllocSpace();

	if (flags & zPAR_FLAG_CLASSVAR)
		return;

	switch (type) {
	case zPAR_TYPE_FLOAT:
	case zPAR_TYPE_INT:
		if (size > 1)
			file->Read(content.data_ptr, 4 * size);
		if (size != 1)
			break;
	case zPAR_TYPE_FUNC:
	case zPAR_TYPE_PROTOTYPE:
	case zPAR_TYPE_INSTANCE:
		file->Read(&this->content, 4);
		break;
	case zPAR_TYPE_STRING: {
		zSTRING str;
		for (int i = 0; i < size; ++i) {
			file->Read(str);
			content.data_pstring[i] = str;
		}
		break;
	}
	case zPAR_TYPE_CLASS:
		// в оригинале разные места
		file->Read(file, &tmp, 4);
		this->content.data_ptr = tmp;
	default:
		break;
	}
}

void zCPar_Symbol::Load(zFILE *file)
{
	int tmp;
	file->Read(file, &tmp, 4);
	if ( tmp )
		file->Read(this->name);
	file->Read(&this->offset, 28);
	this->next = 0;

	if ( flags & zPAR_FLAG_CONST == 0 )
		return;

	int size = bitfield & 0xFFF;

	AllocSpace();
	switch ( type ) {
	case zPAR_TYPE_FLOAT:
	case zPAR_TYPE_INT:
		if (size > 1)
			file->Read(content.data_ptr, 4 * size);
		else if (size == 1)
			file->Read(&this->content, 4);
		break;
	case zPAR_TYPE_STRING: {
		zSTRING str;
		for (int i = 0; i < size; ++i) {
			file->Read(str);
			content.data_pstring[i] = str;
		}
		break;
	}
	case zPAR_TYPE_CLASS:
		file->Read(file, &tmp, 4);
		this->content.data_ptr = tmp;
		break;
	default:
		break;
	}
}

void zCPar_Symbol::SaveFull(zFILE *file)
{
	int hasName = !name.IsEmpty();
	file->Write((const char *)&hasName, 4);
	if ( hasName )
	{
		zSTRING name = this->name + "\n";
		file->Write(name);
	}

	file->Write((const char *)&this->offset, 28);

	if (flags & zPAR_FLAG_CLASSVAR)
		return;

	int size = bitfield & 0xFFF;
	switch ( type ) {
	case zPAR_TYPE_FLOAT:
	case zPAR_TYPE_INT:
		if (size > 1)
			file->Write((const char *)content.data_ptr, 4 * size);
		if (size != 1)
			break;
	case zPAR_TYPE_FUNC:
	case zPAR_TYPE_PROTOTYPE:
	case zPAR_TYPE_INSTANCE:
		file->Write((const char *)&content, 4);
		break;
	case zPAR_TYPE_STRING: {
		zSTRING str;
		for (int i = 0; i < size; ++i) {
			str = *content->data_pstring[i] + "\n";
			file->Write(str);
		}
		break;
	}
	case zPAR_TYPE_CLASS:
		tmp = content.data_ptr;
		file->Write(file, &tmp, 4);
		break;
	default:
		break;
	}
}

void zCPar_Symbol::Save(zFILE *file)
{
	int hasName = !name.IsEmpty();
	file->Write((const char *)&hasName, 4);
	if ( hasName )
	{
		zSTRING name = this->name + "\n";
		file->Write(name);
	}

	file->Write((const char *)&this->offset, 28);

	if (flags & zPAR_FLAG_CONST == 0)
		return;

	int size = bitfield & 0xFFF;
	switch ( type ) {
	case zPAR_TYPE_FLOAT:
	case zPAR_TYPE_INT:
		if (size > 1)
			file->Write((const char *)content.data_ptr, 4 * size);
		else if (size == 1)
			file->Write((const char *)&content, 4);
		break;
	case zPAR_TYPE_STRING: {
		zSTRING str;
		for (int i = 0; i < size; ++i) {
			str = *content->data_pstring[i] + "\n";
			file->Write(str);
		}
		break;
	}
	case zPAR_TYPE_CLASS:
		tmp = content.data_ptr;
		file->Write(file, &tmp, 4);
		break;
	default:
		break;
	}
}

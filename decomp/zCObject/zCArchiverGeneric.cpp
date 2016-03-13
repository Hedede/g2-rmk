void IsTransistent(zCObject* obj)
{
	if (!obj)
		return = true

	auto classdef = obj->_GetClassDef();
	return classdef->classFlags & zCLASS_FLAG_TRANSIENT;
}

void zCArchiverGeneric::WriteObject(const char* name, zCObject* obj)
{
	if ( IsTransistent(obj) ) {
		WriteChunkStart(name, "%", 0, 0);
		WriteChunkEnd();
		return;
	}

	auto entry = writeObjectEntries.Element(obj);

	if (entry && entry->index >= 0) {
		WriteChunkStart(name, "§", 0, entry->index); // [wayl9 § 0 20]
		WriteChunkEnd();
		return;
	}

	WriteObject(name, *obj);
}

void zCArchiverGeneric::WriteWord(char const* name, uint16_t word)
{
	if ( this->unk_1[0] )
	{
		StoreBuffer(&word, sizeof(word));
		return;
	}
	vtable->WriteASCIILine(name, "int", zSTRING(word));
}

void zCArchiverGeneric::WriteASCIILine(const char *name, const char *type, zSTRING const& value)
{
	if ( !IsStringValid(name)  ||
	     !IsStringValid(value) )
	{
		zERROR::Fault("illegal characters: \"" + name + " / " + value);
	}

	StoreBuffer(zTabs, this->indentLevel);// string with lotsa tabs
	StoreString(name);
	StoreBuffer(" =", 1);
	StoreString(type);
	StoreBuffer(" :", 1);
	StoreStringEOL(value);
}

uint16_t zCArchiverGeneric::GetCurrentChunkVersion()
{
	if (chunkRecords.IsEmpty())
		return 0;

	auto chunk = chunkRecords[chunkRecords.NumInList() - 1];
	return chunk.version;
}

zCClassDef* zCArchiverGeneric::GetClassDefByString(zSTRING const& className)
{
	zCClassDef* classDef = nullptr;
	int i = 1;
	do {
		zSTRING word = className.PickWord(className, i++, ":", ":");
		classDef = zCClassDef::GetClassDef(word);
	} while (!classDef && !word.Empty());
	return classDef;
}

zCObject* zCArchiverGeneric::CreateObject(zSTRING const& objName)
{
	zCClassDef* classdef = GetClassDefByString(objname);
	if (!classdef)
		return nullptr;

	zCObject* newinst = classdef->CreateNewInstance();
	if ( !newinst )
		zFAULT("D: zArchiver(zCArchiverGeneric::CreateObject): could not create object for: " + objName);
	return newinst;
}

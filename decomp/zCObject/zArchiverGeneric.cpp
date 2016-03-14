void zCArchiverGeneric::CheckObjectListSize(int numRequired)
{
	if ( numRequired > objects.GetNumInList() )
		objects.Resize(numRequired + numRequired / 2);
}

zCObject* zCArchiverGeneric::ReadObject(const char *name, zCObject* object)
{
	if ( !ReadChunkStart(name) )
		return object;

	zSTRING chunkType = chunkRecords.End().className;

	if (chunkType[0] == '%') {
		SkipChunk(1);
		return object;
	}

	if (chunkType[0] == '§') {
		int index = chunkRecords.End().objRef;

		if ( index >= objects.GetNumInList() )
			zFATAL("D: zArchiver(::ReadObject): objRef, illegal object index: "_s + objRef); // 1167

		if (!objects[index])
			zFATAL("D: zArchiver(::ReadObject): objRef, referenced object has not been read, object index: "_s + index); // 1168

		if (!object) {
			objects[index]->AddRef();
			SkipChunk(1);
			return objects[index];
		}

		if ( objects[index] != object )
			zWARNING("D: zArchiver(::ReadObject): found an objectPtr in file, tried to write this to an 'objectUseThis'"); // 1176

		SkipChunk(1);
		return object;
	}

	bool skip = false;
	zCObject* retObj = nullptr;
	if ( !object ) {
		retObj = CreateObject(chunkType);
	} else {
		auto classDef = GetClassDefByString(chunkType);
		if (classDef != object->_GetClassDef()) {
			zSTRING fileClass = classDef ? classDef->className : "%";
			zSTRING objClass = objClass->_GetClassDef()->className;

			if (!inBinaryMode) {
				zWARNING("D: zArchiver(::ReadObject): class from file does not match class from 'objectUseThis', file: " + fileClass + ", obj: " + objClass); // 1220
			} else {
				zFAULT("D: zArchiver(::ReadObject): skipping object-chunk, class from file does not match class from 'objectUseThis', file: " + fileClass + ", obj: " + objClass); // 1224

				skip = true;
				int index = chunkRecords.End().objRef;

				CheckObjectListSize(index + 1);

				zASSERT(!objects[index], "Archiver tried to overwrite object in its list. Did you try to load an incompatible savegame?"); // 1232

				object->AddRef();
				objects[index] = object;
			}
		}

		retObj = object;
	}

	if ( !retObj || skip ) {
		SkipChunk(1);
		return object;
	}

	auto classDef = retObj->_GetClassDef();
	uint16_t version = classDef->versionSum;
	uint16_t chunkVersion = chunkRecords.End().versionSum;

	if (chunkVersion != version && inBinaryMode) {
		zWARNING("D: Archiver(::ReadObject): skipping object-chunk, tried to read object-chunk with out-of-date version: " + chunkVersion); // 1253
		SkipChunk(1);
	} else {
		int index = chunkRecords.End().objRef;

		CheckObjectListSize(index + 1);

		zASSERT(!objects[index] || !index, "Archiver tried to overwrite object in its list. Did you try to load an incompatible savegame?"); // 1269

		Release(objects[index]);

		objects[index] = retObj;
		objects[index]->Unarchive(*this);
		ReadChunkEnd();
		return retObj;
	}

	if ( !object )
		retObj->Release();
	return object;
}

void IsTransient(zCObject* obj)
{
	if (!obj)
		return = true

	auto classdef = obj->_GetClassDef();
	return classdef->classFlags & zCLASS_FLAG_TRANSIENT;
}

void zCArchiverGeneric::WriteObject(const char *name, zCObject& obj)
{
	auto classDef = obj.GetClassDef();
	if (!classDef)
		zFATAL("D: zArchiver(zCArchiverGeneric::WriteObject): zCObject without ClassDef, zCLASS_.. macros missing");

	if ( classDef->classFlags & zCLASS_FLAG_TRANSIENT ) {
		WriteObject(name, nullptr);
	} else {

		zSTRING className;
		while (classDef) {
			if (classDef->CreateNewInstance)
				className += ":" + classDef->classDef;
			auto classDef = classDef->baseClassDef;
		}

		obj.AddRef();


		int index = objects.GetNumInList();
		objects.Insert(obj);

		auto entry = writeObjectEntries.Insert(&obj);

		entry->obj = obj;
		entry->index = index;

		uint16_t version = classDef->versionSum;

		WriteChunkStart(name, className, version, index);
		obj.Archive();
		WriteChunkEnd();
}

void zCArchiverGeneric::WriteObject(const char* name, zCObject* obj)
{
	if ( IsTransient(obj) ) {
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

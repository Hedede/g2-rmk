zCArchiverGeneric::~zCArchiverGeneric()
{
	Close();
	writeObjectEntries.Clear();
}

void zCArchiverGeneric::RestoreBuffer(void* out, size_t size)
{
	if ( useBuffer )
		buffer->Read(out, outsize);
	else
		file->Read(out, outsize);
}

void zCArchiverGeneric::RestoreString0(zSTRING& out)
{
	RestoreStringEOL(out);
}

int zCArchiverGeneric::RestoreGetPos()
{
	if ( useBuffer )
		return buffer->GetPos();
	return file->Pos();
}

int32_t zCArchvierGeneric::ReadInt(char const* name)
{
	int tmp;
	ReadInt(name, tmp);
	return tmp;
}

void zCArchvierGeneric::ReadInt(char const* name,int& value)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(value));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

uint8_t zCArchvierGeneric::ReadByte(char const* name)
{
	uint8_t tmp;
	ReadByte(name, tmp);
	return tmp;
}

void zCArchvierGeneric::ReadByte(char const* name, uint8_t& byte)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(byte));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

uint16_t zCArchvierGeneric::ReadWord(char const* name)
{
	uint16_t tmp;
	ReadWord(name, tmp);
	return tmp;
}
void zCArchvierGeneric::ReadWord(char const* name,uint16_t& word)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(word));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

void zCArchvierGeneric::ReadFloat(char const* name)
{
	float tmp;
	ReadFloat(name, tmp);
	return tmp;
}

void zCArchvierGeneric::ReadFloat(char const* name, float& value)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(value));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToFloat();
}

bool zCArchvierGeneric::ReadBool(char const* name)
{
	zBOOL val;
	ReadBool(name, val);
	return val;
}

void zCArchvierGeneric::ReadBool(char const* name, zBOOL& val)
{
	if ( inBinaryMode ) {
		val = ReadByte(name);
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

zSTRING zCArchvierGeneric::ReadString(char const* name)
{
	zSTRING str;
	ReadString(name, str);
	return str;
}
void zCArchvierGeneric::ReadString(char const* name, zSTRING& str)
{
	if ( inBinaryMode )
		RestoreString0(str);
	else
		ReadEntryASCII(name, str);
}

zVEC3 zCArchvierGeneric::ReadVec3(char const* name)
{
	zVEC3 vec;
	ReadVec3(name, vec);
	return vec;
}
void zCArchvierGeneric::ReadVec3(char const* name,zVEC3& vec)
{
	if ( inBinaryMode ) {
		enu = ReadByte(name);
		return;
	}

	if (ReadEntryASCII(name, tempString)) {
		float r, g, b, a;
		sscanf(tempstr.Data(), "%f %f %f", &x, &y, &z);
	}
}

zCOLOR zCArchvierGeneric::ReadColor(char const* name)
{
	zCOLOR color;
	ReadColor(name, color);
	return color;
}

void zCArchvierGeneric::ReadColor(char const* name, zCOLOR& color)
{
	if ( inBinaryMode ) {
		enu = ReadByte(name);
		return;
	}

	if (ReadEntryASCII(name, tempString)) {
		uint32_t r, g, b, a;
		sscanf(tempstr.Data(), "%d %d %d %d", &r, &g, &b, &a);

		color[2] = r;
		color[1] = g;
		color[0] = b;
		color[3] = a;
	}
}

uint32_t zCArchvierGeneric::ReadEnum(char const* name)
{
	uint32_t enu;
	ReadEnum(name, enu);
	return enu;
}

void zCArchvierGeneric::ReadEnum(char const* name, ulong enu)
{
	if ( inBinaryMode ) {
		enu = ReadByte(name);
		return;
	}

	if (ReadEntryASCII(name, tempString))
		enu = tempString.ToInt();
}

virtual void ReadRaw(char const* name, void* out, size_t size)
{
	if ( inBinaryMode ) {
		RestoreBuffer(out, size);
		return;
	}

	ReadEntryASCII(name, tempString);
	size_t avail = tempString.Length() / 2;
	if ( size != avail )
		zFAULT("D: zArchiver(zCArchiverGeneric::ReadRaw): size of requested data does not equal size of available data: " + size + "/" + avail); // 1468

	char* output = static_cast<char*>(out);
	char const* data = tempString.Data();
	while (avail) {
		uint8_t byte;
		sscanf(data, "%2x", &byte);

		*output++ = byte;

		data += 2;

		--avail;
	}
}

virtual void ReadRawFloat(char const* name, void* out, size_t size)
{
	if ( inBinaryMode )
	{
		RestoreBuffer(out, size);
		return;
	}

	ReadEntryASCII(name, tempString);

	size_t num = size / 4;
	for (size_t i = 0; i < num; ++i) {
		zSTRING word = tempString.PickWord(i, " ", " ");
		*out++ = word.ToFloat();
	}
}

void zCArchiverGeneric::StoreBuffer(void* buf, size_t size)
{
	if ( size ) {
		if ( useBuffer )
			buffer->Write(buf, size);
		else
			file->Write(buf, size);
	}
}

void zCArchiverGeneric::StoreString(const char* str)
{
	size_t len = strlen(str);
	StoreBuffer(str, len);
}

void zCArchiverGeneric::StoreStringEOL(const char *a2, const char *a3)
{
	StoreString(str);
	StoreString(stringEOL.Data());
}

void zCArchiverGeneric::WriteInt(const char *name, int32_t value)
{
	if ( inBinaryMode ) {
		StoreBuffer(&value, sizeof(value));
		return;
	}

	WriteASCIILine(name, "int", zSTRING(value));
}

void zCArchiverGeneric::WriteWord(char const* name, uint16_t word)
{
	if ( inBinaryMode ) {
		StoreBuffer(&word, sizeof(word));
		return;
	}
	WriteASCIILine(name, "int", zSTRING(word));
}

void zCArchiverGeneric::WriteByte(const char* name, uint8_t value)
{
	if ( inBinaryMode ) {
		StoreBuffer(&value, sizeof(value));
		return;
	}

	WriteASCIILine(name, "int", zSTRING(value));
}

void zCArchiverGeneric::WriteBool(const char* name, uint8_t value)
{
	if ( inBinaryMode ) {
		StoreBuffer(&value, sizeof(value));
		return;
	}

	WriteASCIILine(name, "bool", zSTRING(value));
}

void zCArchiverGeneric::WriteWord(char const* name, zCOLOR const& value)
{
	if ( inBinaryMode ) {
		StoreBuffer(&word, sizeof(value));
		return;
	}
	WriteASCIILine(name, "color", value.GetDescription());
}

void zCArchiverGeneric::WriteEnum(const char *name, const char *enumChoices, int value)
{
	if ( inBinaryMode ) {
		WriteByte(name, value);
		return;
	}

	if ( InProperties() ) {
		WriteASCIILine(name, "enum;" + enumChoices, zSTRING(value));
		return;
	}

	WriteASCIILine(name, "enum", zSTRING(value));
}

void zCArchiverGeneric::WriteFloat(const char *name, float value)
{
	if ( inBinaryMode ) {
		WriteByte(name, value);
		return;
	}

	WriteASCIILine(name, "float", zSTRING(value));
}


void zCArchiverGeneric::WriteVec3(char const* name, zVEC3 const& value)
{
	if ( inBinaryMode ) {
		StoreBuffer(&value, sizeof(value));
		return;
	}
	WriteASCIILine(name, "vec3", value.GetDescription());
}

void zCArchiverGeneric::WriteString(const char *name, zSTRING const& value)
{
	if ( inBinaryMode ) {
		StoreString(value.Data());
		StoreBuffer(byte_8D482C, 1);// I?
		return;
	}

	WriteASCIILine(name, "string", value);
}

void zCArchiverGeneric::WriteRaw(const char* name, void* val, size_t size)
{
	if ( inBinaryMode ) {
		StoreBuffer(val, size);
		return;
	}

	zSTRING raw;
	char* ptr = static_cast<char*>(val);
	for (size_t s = 0; s < size; ++s) {
		char buf[3] = {};
		sprintf(buf, "%02x", ptr[s]);
		raw += buf;
	}

	WriteASCIILine(name, "raw", raw);
}

void zCArchiverGeneric::WriteRawFloat(const char* name, void* src, size_t size)
{
	if ( size & 3 ) {
		zFATAL("D: zArchiver(zCArchiverGeneric::WriteRawFloat): raw float buffer has illegal size (mod4!=0): "_s + size); // 557
	}

	if ( inBinaryMode ) {
		StoreBuffer(val, size);
		return;
	}

	float* floats = static_cast<float*>(src);
	zSTRING raw;
	size_t count = size / 4;
	for (size_t i; i < count; ++i )
		raw += zSTRING(floats[i]);

	WriteASCIILine(name, "rawFloat", raw);
}


char const* spaces = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

void zCArchiverGeneric::WriteASCIILine(const char *name, const char *type, zSTRING const& value)
{
	if ( !IsStringValid(name)  ||
	     !IsStringValid(value) )
	{
		zERROR::Fault("illegal characters: \"" + name + " / " + value);
	}

	StoreBuffer(spaces, chunkRecords.GetNumInList());
	StoreString(name);
	StoreBuffer(" =", 1);
	StoreString(type);
	StoreBuffer(" :", 1);
	StoreStringEOL(value);
}



void zCArchiverGeneric::CheckObjectListSize(int numRequired)
{
	if ( numRequired > objectList.GetNumInList() )
		objectList.Resize(numRequired + numRequired / 2);
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

		if ( index >= objectList.GetNumInList() )
			zFATAL("D: zArchiver(::ReadObject): objRef, illegal object index: "_s + objRef); // 1167

		if (!objectList[index])
			zFATAL("D: zArchiver(::ReadObject): objRef, referenced object has not been read, object index: "_s + index); // 1168

		if (!object) {
			objectList[index]->AddRef();
			SkipChunk(1);
			return objectList[index];
		}

		if ( objectList[index] != object )
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

				zASSERT(!objectList[index], "Archiver tried to overwrite object in its list. Did you try to load an incompatible savegame?"); // 1232

				object->AddRef();
				objectList[index] = object;
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
	uint16_t chunkVersion = GetCurrentChunkVersion();

	if (chunkVersion != version && inBinaryMode) {
		zWARNING("D: Archiver(::ReadObject): skipping object-chunk, tried to read object-chunk with out-of-date version: " + chunkVersion); // 1253
		SkipChunk(1);
	} else {
		int index = chunkRecords.End().objRef;

		CheckObjectListSize(index + 1);

		zASSERT(!objectList[index] || !index, "Archiver tried to overwrite object in its list. Did you try to load an incompatible savegame?"); // 1269

		Release(objectList[index]);

		objectList[index] = retObj;
		objectList[index]->Unarchive(*this);
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


		int index = objectList.GetNumInList();
		objectList.Insert(obj);

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

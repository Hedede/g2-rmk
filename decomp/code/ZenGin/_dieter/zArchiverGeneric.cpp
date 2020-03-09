#include "ZenGin/_dieter/zArchiverGeneric.h"
#include "ZenGin/_dieter/zContainer.h"
#include "ZenGin/_dieter/zSparseArray.h"

zCArchiverGeneric::zCArchiverGeneric()
	: zCArchiver(), writeObjectEntries(0)
{
	writeObjectEntries.CreateObject(0x407);
}

zCArchiverGeneric::~zCArchiverGeneric()
{
	Close();
	writeObjectEntries.Clear();
}

void zCArchiverGeneric::OpenWriteFile(zFILE* fileWrite, zTArchiveMode arcMode, int in_sg, int flags, int owns_medium)
{
	if ( fileWrite ) {
		Close();

		inSaveGame = in_sg;
		mode = arcMode;
		useBuffer = 0;
		rwMode = 2;
		file = fileWrite;
		inBinaryMode = mode == 0;
		arcOwnsMedium = owns_medium;
		arcFlags = flags;
		notInProperties = mode != ARC_MODE_ASCII_PROPS;

		WriteHeader(flags);
	}
}

void zCArchiverGeneric::OpenReadFile(zFILE* fileRead, zTArchiveMode arcMode, int in_sg, int flags, int owns_medium)
{
	if ( fileRead ) {
		Close();

		inSaveGame = in_sg;
		mode = arcMode;
		useBuffer = 0;
		rwMode = 1;
		file = fileWrite;
		inBinaryMode = mode == ARC_MODE_BINARY;
		arcOwnsMedium = owns_medium;
		// arcFlags = flags;

		ReadHeader();
		inBinaryMode = mode == ARC_MODE_BINARY;
		notInProperties = mode != ARC_MODE_ASCII_PROPS;
	}
}

int zCArchiverGeneric::OpenWriteBuffer(zCBuffer* buf, zTArchiveMode arcMode, int saveGame, int flags, int ownsMedium)
{
	Close();

	useBuffer = 1;
	rwMode = 2;
	mode = arcMode;
	inSaveGame = saveGame;
	arcOwnsMedium = ownsMedium;
	arcFlags = arcFlags;
	inBinaryMode = mode == ARC_MODE_BINARY;
	notInProperties = mode != ARC_MODE_ASCII_PROPS;
	buffer = buf;
	buffer->SetMode(1);

	DebugMessage("D: ARC: Writing archive buffer (" + s_arcModeNames[arcMode] + ") ..");
	WriteHeader(arcFlags);
	return 1;
}

int zCArchiverGeneric::OpenReadBuffer(zCBuffer *buf, zTArchiveMode arcMode, int saveGame, int flags)
{
	Close();
	useBuffer = 1;
	rwMode = 1;
	inSaveGame = saveGame;
	arcOwnsMedium = 0;
	arcFlags = flags;
	mode = arcMode;

	inBinaryMode = mode == ARC_MODE_BINARY;

	buffer = buf;
	buffer->SetMode(0);

	if ( EndOfArchive() ) {
		Close();
		return 0;
	}
	ReadHeader();

	inBinaryMode = mode == ARC_MODE_BINARY;
	notInProperties = mode != ARC_MODE_ASCII_PROPS;

	DebugMessage("D: ARC: Reading archive buffer (" + s_arcModeNames[arcMode] + ") ..");

	return 1;
}

void zCArchiverGeneric::DeleteBuffer()
{
	if (arcOwnsMedium)
		Delete(buffer);
	buffer = 0
}

void zCArchiverGeneric::Close()
{
	if ( rwMode == 0 )
		return;

	if ( rwMode == 2 )
		WriteHeaderNumObj();

	if ( arcOwnsMedium ) {
		DebugMessage("D: ARC: .. Closing ("_s + objectList.GetNum() + " objects)");

		if ( useBuffer ) {
			DeleteBuffer();
		} else if ( file ) {
			if ( file->IsOpened() )
				file->close();
			if ( arcOwnsMedium )
				delete file;
			file = 0;
		}
	}

	for (auto& object : objectList)
		Release(object);

	objectList.Clear();
	chunkRecords.Clear();
	writeObjectEntries.Clear();

	file = 0;
	buffer = 0;
	rwMode = 0;
}

void zCArchiverGeneric::RestoreBuffer(void* out, size_t size)
{
	if ( useBuffer )
		buffer->Read(out, outsize);
	else
		file->Read(out, outsize);
}

void zCArchiverGeneric::RestoreStringEOL(zSTRING& resultLine)
{
	int i = 0;
	char tmpBuf[5000];
	char peg = 0;
	useBuffer = this->useBuffer;
	if ( inBinaryMode ) {
		if ( useBuffer ) {
			buffer->Read(tmpBuf, 1);
			if ( tmpBuf[0] ) {
				do {
					if ( tmpBuf[i] == '\n' )
						break;
					if ( ++i == 5000 ) {
						peg = 0;
						resStr += tmpBuf;
						i = 0;
					}
					buffer->Read(tmpBuf + i, 1);
				} while ( tmpBuf[i] );
			}
		} else {
			file->Read(tmpBuf, 1);
			if ( tmpBuf[0] ) {
				do {
					if ( tmpBuf[i] == '\n' )
						break;
					if ( ++i == 5000 ) {
						peg = 0;
						resStr += tmpBuf;
						i = 0;
					}
					file->Read(tmpBuf + i, 1);
				} while ( tmpBuf[i] );

			}
		}
	} else if ( useBuffer ) {
		buffer->Read(tmpBuf, 1);
		if ( tmpBuf[0] != '\n' ) {
			do {
				if ( tmpBuf[i] == '\r' )
					break;
				if ( tmpBuf[i] != '\t' )
					++i;
				if ( i == 5000 ) {
					peg = 0;
					resStr += tmpBuf;
					i = 0;
				}
				buffer->Read(tmpBuf + i, 1);
			}
			while ( tmpBuf[i] != '\n' );
		}
	} else {
		file->Read(tmpBuf, 1);
		if ( tmpBuf[0] != '\n' ) {
			do {
				if ( tmpBuf[i] == '\r' )
					break;
				if ( tmpBuf[i] != '\t' )
					++i;
				if ( i == 5000 ) {
					peg = 0;
					resStr += tmpBuf;
					i = 0;
				}
				file->Read(tmpBuf + i, 1);
			}
			while ( tmpBuf[i] != '\n' );
		}
	}

	tmpBuf[i] = 0;
	resStr += tmpBuf;
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

void zCArchiverGeneric::RestoreSeek(size_t pos)
{
	if ( pos != RestoreGetPos() ) {
		if ( useBuffer )
			buffer->SetPos(pos);
		else
			file->Seek(pos);
	}
}

int zCArchiverGeneric::EndOfArchive()
{
	if ( useBuffer )
		return buffer->GetPos() > buffer->GetSize(); // >= ?

	if (!file->Eof()) {
		if (file->Pos() + 3 <= file->Size())
			return 0;
	}

	return 1;
}

int32_t zCArchiverGeneric::ReadInt(char const* name)
{
	int tmp;
	ReadInt(name, tmp);
	return tmp;
}

void zCArchiverGeneric::ReadInt(char const* name,int& value)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(value));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

uint8_t zCArchiverGeneric::ReadByte(char const* name)
{
	uint8_t tmp;
	ReadByte(name, tmp);
	return tmp;
}

void zCArchiverGeneric::ReadByte(char const* name, uint8_t& byte)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(byte));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

uint16_t zCArchiverGeneric::ReadWord(char const* name)
{
	uint16_t tmp;
	ReadWord(name, tmp);
	return tmp;
}
void zCArchiverGeneric::ReadWord(char const* name,uint16_t& word)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(word));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

void zCArchiverGeneric::ReadFloat(char const* name)
{
	float tmp;
	ReadFloat(name, tmp);
	return tmp;
}

void zCArchiverGeneric::ReadFloat(char const* name, float& value)
{
	if ( inBinaryMode ) {
		RestoreBuffer(value, sizeof(value));
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToFloat();
}

bool zCArchiverGeneric::ReadBool(char const* name)
{
	zBOOL val;
	ReadBool(name, val);
	return val;
}

void zCArchiverGeneric::ReadBool(char const* name, zBOOL& val)
{
	if ( inBinaryMode ) {
		val = ReadByte(name);
		return;
	}

	if (ReadEntryASCII(name, tempString))
		value = tempString.ToInt();
}

zSTRING zCArchiverGeneric::ReadString(char const* name)
{
	zSTRING str;
	ReadString(name, str);
	return str;
}
void zCArchiverGeneric::ReadString(char const* name, zSTRING& str)
{
	if ( inBinaryMode )
		RestoreString0(str);
	else
		ReadEntryASCII(name, str);
}

zVEC3 zCArchiverGeneric::ReadVec3(char const* name)
{
	zVEC3 vec;
	ReadVec3(name, vec);
	return vec;
}
void zCArchiverGeneric::ReadVec3(char const* name,zVEC3& vec)
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

zCOLOR zCArchiverGeneric::ReadColor(char const* name)
{
	zCOLOR color;
	ReadColor(name, color);
	return color;
}

void zCArchiverGeneric::ReadColor(char const* name, zCOLOR& color)
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

uint32_t zCArchiverGeneric::ReadEnum(char const* name)
{
	uint32_t enu;
	ReadEnum(name, enu);
	return enu;
}

void zCArchiverGeneric::ReadEnum(char const* name, ulong enu)
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

int zCArchiverGeneric::StoreGetPos()
{
	if ( useBuffer )
		return = buffer->GetPos(this->buffer);
	return file->Pos();
}


void zCArchiverGeneric::StoreSeek(size_t pos)
{
	if ( useBuffer )
		buffer->SetPos(pos);
	else
		file->Seek(pos);
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


char const spaces[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

void zCArchiverGeneric::WriteASCIILine(const char *name, const char *type, zSTRING const& value)
{
	if ( !IsStringValid(name) || !IsStringValid(value) )
		zERROR::Fault("illegal characters: \"" + name + " / " + value);

	StoreBuffer(spaces, chunkRecords.GetNum());
	StoreString(name);
	StoreBuffer(" =", 1);
	StoreString(type);
	StoreBuffer(" :", 1);
	StoreStringEOL(value);
}

void zCArchiverGeneric::WriteGroupEnd(const char *name)
{
	if ( InProperties() )
		WriteASCIILine(name, "groupBegin", "");
}

void zCArchiverGeneric::WriteGroupEnd(const char *name)
{
	if ( InProperties() )
		WriteASCIILine(name, "groupEnd", "")
}

void zCArchiverGeneric::CheckObjectListSize(int numRequired)
{
	if ( numRequired > objectList.GetNum() )
		objectList.Resize(numRequired + numRequired / 2);
}

void zCArchiverGeneric::WriteChunkEnd()
{
	auto numrec = chunkRecords.GetNum();
	if ( numrec == 0 )
		zFATAL("D: zArchiver(zCArchiverGeneric::WriteChunkEnd): write chunk end with no open chunk !"); // 625,

	zTChunkRecord record = chunkRecords[numrec - 1];
	/* shto za dermo
	v7 = chunkRecords.GetNum();
	v8 = v7 - 1;
	if ( v7 - 1 < v7 ) {
		v9 = v7 - 1;
		if ( v8 != v9 ) {
			v10 = v8;
			if ( v8 < v9 ) {
				v11 = v10;
				do {
				} while (v11 < chunkRecords.GetNum() - 1);
			}
		}
	}*/

	chunkRecords.RemoveOrderIndex(numrec - 1);

	if ( inBinaryMode ) {
		auto end = StoreGetPos();
		size_t len = end - record.start;
		auto savedpos = StoreGetPos();
		StoreSeek(record.start);
		StoreBuffer(&len, 4);
		StoreSeek(savedpos);
	} else {
		StoreBuffer(spaces, chunkRecords.GetNum());
		StoreStringEOL("[]");
	}
}

void zCArchiverGeneric::ReadChunkStartASCII(const char *chunkName, zSTRING& resultLine)
{
	auto savedpos = RestoreGetPos();
	resultLine.Clear();

	zSTRING str;
	while ( !EndOfArchive() ) {
		RestoreStringEOL(str);
		if (str[0] != '[')
			continue;
		if (str[1] == ']') {
			auto pos = savedpos;
			if ( !noReadSearchCycles) {
				auto idx = chunkRecords.GetNum();
				if (idx > 0)
					pos = chunkRecords[idx].start;
			}

			RestoreSeek(pos);
			continue;
		}

		auto word = str.PickWord(1u, ZSTR_LBRACKET, ZSTR_LBRACKET);
		zSTRING cname = chunkName;

		if (cname.IsEmpty() || word == cname ) {
			resultLine = str;
			return;
		}

		SkipChunk(0);
	}
}

int zCArchiverGeneric::ReadChunkStart(const char *chunkName)
{
	zCArchiver::zTChunkRecord chunk;
	if ( inBinaryMode ) {
		chunk.start = RestoreGetPos();
		RestoreBuffer(&chunk.length, 4);
		RestoreBuffer(&chunk.versionSum, 2);
		RestoreBuffer(&chunk.objectIndex, 4);
		RestoreString0(&chunk.className);
		RestoreString0(&chunk.chunkName);
		if ( chunkName && chunkName != chunk.className ) {
			RestoreSeek(chunk.start);
			return 0;
		}
	} else {
		zSTRING chunkHead;
		ReadChunkStartASCII(chunkName, chunkHead);
		if ( chunkHead.IsEmpty() )
			return 0;

		chunk.start = RestoreGetPos();
		chunk.className   = chunkHead.PickWord(1, " [", " [");
		chunk.chunkName   = chunkHead.PickWord(2, " ",  " ");
		chunk.versionSum  = chunkHead.PickWord(3, " ",  " ");
		chunk.objectIndex = chunkHead.PickWord(4, " ]", " ]");
	}

	chunkRecords.InsertEnd(chunk);
	return 1;
}

int zCArchiverGeneric::ReadChunkStart(zSTRING& chunkName, uint16_t& chunkVersion)
{
	int result = ReadChunkStart("");
	if ( result ) {
		// again, compiler optimized this into bullshit
		chunkName    = chunkRecords[chunkVersion.GetNum()-1].chunkName;
		chunkVersion = chunkRecords[chunkRecords.GetNum()-1].chunkVersion;
		return result;
	}

	chunkName.Clear();
	return 0;
}

int zCArchiverGeneric::ReadChunkStartNamed(const char* chunkName, short& chunkVersion)
{
	int result = ReadChunkStart(chunkName);
	// not sure, compiler optimized this into
	// chunkRecords[chunkRecords.GetNum()] - 24, which is not .chunkVersion
	if ( result )
		chunkVersion = chunkRecords[chunkRecords.GetNum()-1].chunkVersion;
	return result;
}


zCObject* zCArchiverGeneric::ReadObject(const char *name, zCObject* object)
{
	if ( !ReadChunkStart(name) )
		return object;

	int numChunks = chunkRecords.GetNum();
	zSTRING chunkType = chunkRecords[numChunks-1].className;

	if (chunkType[0] == '%') {
		SkipOpenChunk();
		return object;
	}

	if (chunkType[0] == '§') {
		int index = chunkRecords[numChunks-1].objectIndex;

		if ( index >= objectList.GetNum() )
			zFATAL("D: zArchiver(::ReadObject): objRef, illegal object index: "_s + index); // 1167

		if (!objectList[index])
			zFATAL("D: zArchiver(::ReadObject): objRef, referenced object has not been read, object index: "_s + index); // 1168

		if (!object) {
			objectList[index]->AddRef();
			SkipOpenChunk();
			return objectList[index];
		}

		if ( objectList[index] != object )
			zWARNING("D: zArchiver(::ReadObject): found an objectPtr in file, tried to write this to an 'objectUseThis'"); // 1176

		SkipOpenChunk();
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
				int index = chunkRecords[numChunks-1].objectIndex;

				CheckObjectListSize(index + 1);

				zASSERT(!objectList[index], "Archiver tried to overwrite object in its list. Did you try to load an incompatible savegame?"); // 1232

				object->AddRef();
				objectList[index] = object;
			}
		}

		retObj = object;
	}

	if ( !retObj || skip ) {
		SkipOpenChunk();
		return object;
	}

	auto classDef = retObj->_GetClassDef();
	uint16_t version = classDef->versionSum;
	uint16_t chunkVersion = GetCurrentChunkVersion();

	if (chunkVersion != version && inBinaryMode) {
		zWARNING("D: Archiver(::ReadObject): skipping object-chunk, tried to read object-chunk with out-of-date version: " + chunkVersion); // 1253
		SkipOpenChunk();
	} else {
		int index = chunkRecords[numChunks-1].objectIndex;

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


		int index = objectList.GetNum();
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

void zCArchiverGeneric::WriteHeader(int)
{
	StoreString("objects ");
	pos_headerNumObj = StoreGetPos();

	StoreStringEOL("0        ");
	StoreStringEOL("END");
	StoreStringEOL();

	pos_headerEnd = StoreGetPos();
}

void zCArchiverGeneric::WriteHeaderNumObj()
{
	if ( rwMode == 2 && (useBuffer || file->IsOpened()) ) {
		auto savedpos = StoreGetPos();
		StoreSeek(pos_headerNumObj);
		zSTRING numobj = zSTRING(objectList.GetNum());
		StoreString( numobj );
		StoreSeek(savedpos);
	}
}

void zCArchiverGeneric::ReadHeader()
{
	zSTRING str;
	while (!str)
		RestoreStringEOL(str);

	auto word1 = str.PickWord(1, " ", " ");
	auto word2 = str.PickWord(2, " ", " ");
	if ( word1 == "objects" ) {
		auto num = word2.ToLong();
		if ( num > objectList.GetNum() )
			objectList.Resize(num);
	} else {
		zFATAL("D: zCArchiverGeneric: cannot read archive, 'objects' keyword expected"); // 963,
	}

	RestoreStringEOL(str);
	RestoreStringEOL(str);

	pos_headerEnd = RestoreGetPos();
}

void zCArchiverGeneric::GetBufferString(zSTRING& str)
{
	if ( buffer && mode == ARC_MODE_BINARY ) {
		WriteHeaderNumObj();
		buffer->Write(byte_8D482C, 1u); // some C runtime thing
		buffer->SetPosBegin();
		str = buffer->GetCursor();
	}
}


uint16_t zCArchiverGeneric::GetCurrentChunkVersion()
{
	if (chunkRecords.IsEmpty())
		return 0;

	auto chunk = chunkRecords[chunkRecords.GetNum() - 1];
	return chunk.version;
}

void zCArchiverGeneric::SkipChunk(int openChunk)
{
	int numChunks = chunkRecords.GetNum();
	if (numChunks <= 0)
		return;

	if ( inBinaryMode ) {
		auto& record = chunkRecords[numChunks - 1];
		RestoreSeek(record.length + record.start);
	} else {
		zSTRING chunkhead;
		size_t chunksToSkip  = 1;
		size_t chunksSkipped = 0;

		RestoreStringEOL(chunkhead);

		while (chunksToSkip > 0) {
			if (chunkhead[0] == '[') {
				if (chunkhead[1] == ']')
					--chunksToSkip;
				else
					++chunksToSkip;
			}

			if ( ++chunksSkipped > 1 )
				__skipped_some_chunks = 1;
		}
	}

	if ( openChunk )
		chunkRecords.RemoveOrderIndex(numChunks - 1);
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

zCObject* zCArchiverGeneric::GetParentObject()
{
	zCObject* result = nullptr;
	for (auto i = chunkRecords.GetNum() - 2; i > 0; --i) {
		zTChunkRecord& cr = chunkRecords[i];
		if ( cr.chunkName.Length() > 0 ) {
			if (cr.chunkName[0] != '%')
				break;
		}
		result = objectList[chunkRecords[i].objRef];
	}
	return result;
}

using Tester = void (*)(zCObject*, int, zCClassDef*, int *);
void zCArchiverGeneric::TestClassIntegrity(Tester tester)
{
}


int zCArchiverGeneric::DebugMessage(zSTRING const& msg)
{
	if (notInProperties) {
		if (!(arcFlags & 2))
			return zERROR::Message(msg);
	}
	return 0;
}

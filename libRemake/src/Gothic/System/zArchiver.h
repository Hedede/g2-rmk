#pragma once

#include <Gothic/Types/zCObject.h>

enum zTArchiveMode {
	zARC_MODE_BINARY,
	zARC_MODE_ASCII,
	zARC_MODE_ASCII_PROPS,
	zARC_MODE_BIN_SAFE,
};

class zCBuffer;
class zCArchiver;
struct zCArchiver_vt
{
  zCClassDef *(__thiscall *_GetClassDef)(zCObject *);
  void (__thiscall *Archive)(zCObject *, zCArchiver *);
  void (__cdecl *Unarchive)(zCObject *, zCArchiver *);
  void (__thiscall *dtor)(zCArchiver *, int);
  void (__fastcall *WriteInt)(zCArchiver *, const char *, int);
  void (__fastcall *WriteByte)(zCArchiver *, const char *, char);
  void (__fastcall *WriteWord)(zCArchiver *, const char *, short);
  void (__fastcall *WriteFloat)(zCArchiver *, const char *, float);
  void (__fastcall *WriteBool)(zCArchiver *, const char *, int);
  void (__fastcall *WriteString)(zCArchiver *, const char *, const zSTRING *);
  void (__fastcall *WriteVec3)(zCArchiver *, const char *, const zVEC3 *);
  void (__fastcall *WriteColor)(zCArchiver *, const char *, const zCOLOR *);
  void (__fastcall *WriteRaw)(zCArchiver *, const char *, void *, __int32);
  void (__fastcall *WriteRawFloat)(zCArchiver *, const char *, void *, __int32);
  void (__fastcall *WriteEnum)(zCArchiver *, const char *, const char *, int);
  void (__fastcall *WriteObject_r)(zCArchiver *, const char *, zCObject *);
  void (__fastcall *WriteObject_cp)(zCArchiver *, const char *, zCObject *);
  void (__fastcall *WriteObject_p)(zCArchiver *, zCObject *);
  void (__fastcall *WriteChunkStart)(zCArchiver *, const char *, const char *, short, short);
  void (__fastcall *WriteChunkStart2)(zCArchiver *, const char *, short);
  void (__fastcall *WriteChunkEnd)(zCArchiver *);
  void (__fastcall *WriteGroupBegin)(zCArchiver *, const char *);
  void (__fastcall *WriteGroupEnd)(zCArchiver *, const char *);
  int (__fastcall *ReadInt)(zCArchiver *, const char *);
  void (__fastcall *ReadInt_r)(zCArchiver *, const char *, int *);
  char (__fastcall *ReadByte)(zCArchiver *, const char *);
  void (__fastcall *ReadByte2)(zCArchiver *, const char *, char *);
  short (__fastcall *ReadWord)(zCArchiver *, const char *);
  void (__fastcall *ReadWord2)(zCArchiver *, const char *, short *);
  float (__fastcall *ReadFloat)(zCArchiver *, const char *);
  void (__fastcall *ReadFloat2)(zCArchiver *, const char *, float *);
  int (__fastcall *ReadBool)(zCArchiver *, const char *);
  void (__fastcall *ReadBool2)(zCArchiver *, const char *, int *);
  zSTRING *(__fastcall *ReadString)(zCArchiver *, zSTRING *retstr, const char *);
  zSTRING *(__fastcall *ReadString2)(zCArchiver *, const char *, zSTRING *);
  zVEC3 *(__fastcall *ReadVec3)(zVEC3 * retstr, zCArchiver *, const char *);
  void (__fastcall *ReadVec3_2)(zCArchiver *, const char *, zVEC3 *);
  zCOLOR (__fastcall *ReadColor)(zCArchiver *, const char *);
  void (__fastcall *ReadColor2)(zCArchiver *, const char *, zCOLOR *);
  int (__fastcall *ReadEnum)(zCArchiver *, const char *);
  void (__fastcall *ReadEnum2)(zCArchiver *, const char *, int *);
  void (__fastcall *ReadRaw)(zCArchiver *, const char *, void *, int);
  void (__fastcall *ReadRawFloat)(zCArchiver *, const char *, void *, int);
  zCObject *(__fastcall *ReadObject)(zCArchiver *, const char *, zCObject *);
  void **(__fastcall *ReadObject_n)(zCArchiver *, zCObject *);
  void (__fastcall *ReadObjectAccount)(zCArchiver *, const char *, int, const char *, zCObject *);
  void (__fastcall *ReadObjectAccount2)(zCArchiver *, const char *, int, zCObject *);
  int (__fastcall *ReadChunkStart)(zCArchiver *, const char *);
  int (__fastcall *ReadChunkStart_v)(zCArchiver *, zSTRING *, short *);
  void (__fastcall *ReadChunkStartNamed)(zCArchiver *, const char *, short *);
  void (__fastcall *SkipOpenChunk)(zCArchiver *);
  unsigned short (__fastcall *GetCurrentChunkVersion)(zCArchiver *);
  zFILE *(__fastcall *GetFile)(zCArchiver *);
  void (__fastcall *GetBufferString)(zCArchiver *, zSTRING *);
  zCBuffer *(__fastcall *GetBuffer)(zCArchiver *);
  int (__thiscall *EndOfArchive)(zCArchiver *);
  void (__thiscall *Close)(zCArchiver *);
  void (__fastcall *SetStringEOL)(zCArchiver *, const zSTRING *);
  void (__fastcall *GetStringEOL)(zCArchiver *);
  int (__thiscall *IsStringValid)(zCArchiver *, const char *);
  void (__fastcall *GetChecksumEnabled)(zCArchiver *);
  void (__fastcall *SetChecksumEnabled)(zCArchiver *, int);
  void (__thiscall *SetNoReadSearchCycles)(zCArchiver *, int);
  int (__thiscall *InProperties)(zCArchiver *);
  int (__thiscall *InSaveGame)(zCArchiver *);
  int (__fastcall *InBinaryMode)(zCArchiver *);
  zCObject *(__fastcall *GetParentObject)(zCArchiver *);
  void (__thiscall *OpenWriteBuffer)(zCArchiver *, zCBuffer *, int, int, int, int);
  void (__thiscall *OpenWriteFile)(zCArchiver *, zFILE *, int, int, int, int);
  void (__fastcall *WriteHeader)(zCArchiver *, int);
  void (__thiscall *WriteHeaderNumObj)(zCArchiver *);
  void (__fastcall *WriteASCIILine)(zCArchiver *, const char *, const char *, const zSTRING *);
  void (__fastcall *StoreBuffer)(zCArchiver *, void *, __int32);
  void (__fastcall *StoreString)(zCArchiver *, const char *);
  void (__fastcall *StoreStringEOL)(zCArchiver *, const char *);
  int (__thiscall *StoreGetPos)(zCArchiver *);
  void (__fastcall *StoreSeek)(zCArchiver *, __int32);
  void (__thiscall *OpenReadBuffer)(zCArchiver *, zCBuffer *, int, int, int);
  void (__thiscall *OpenReadFile)(zCArchiver *, zFILE *, int, int, int, int);
  zCClassDef *(__fastcall *GetClassDefByString)(zCArchiver *, const zSTRING *);
  zCObject *(__fastcall *CreateObject)(zCArchiver *, const zSTRING *);
  void (__fastcall *SkipChunk)(zCArchiver *, int);
  void (__fastcall *ReadChunkStartASCII)(zCArchiver *, const char *, zSTRING *);
  void (__fastcall *ReadChunkEnd)(zCArchiver *);
  int (__fastcall *ReadEntryASCII)(zCArchiver *, const char *, zSTRING *);
  void (__fastcall *ReadHeader)(zCArchiver *);
  void (__fastcall *RestoreBuffer)(zCArchiver *, void *, __int32);
  void (__fastcall *RestoreStringEOL)(zCArchiver *, zSTRING *);
  void (__fastcall *RestoreString0)(zCArchiver *, zSTRING *);
  int (__thiscall *RestoreGetPos)(zCArchiver *);
  void (__fastcall *RestoreSeek)(zCArchiver *, __int32);
  void (__fastcall *DeleteBuffer)(zCArchiver *);
};


class zCArchiver : public zCObject
{
public:
	void Close()
	{
		reinterpret_cast<zCArchiver_vt*>(_vtab)->Close(this);
	}
	void ReadObject(zCObject* object)
	{
		reinterpret_cast<zCArchiver_vt*>(_vtab)->ReadObject_n(this, object);
	}
};

#include "zCBuffer.h"

void zCBuffer::zCBuffer()
{
	Init();
}

zCBuffer(size_t len_byte)
{
	Init();
	buffer = new zBYTE[len_byte]; // 90
	allocated = true;
	mode = WRITE;
	maxsize_byte = len_byte;
	maxsize_bit  = 8 * len_byte;
}

zCBuffer(void* b, const size_t& len_byte)
{
	Init();
	buffer = (zBYTE*)b;
	size_byte = len_byte;
	maxsize_byte = len_byte;
	size_bit = len_byte * 8;
	maxsize_byte = len_byte * 8;
}


void zCBuffer::Init()
{
	mode = READWRITE;
	buffer = nullptr;
	pos_byte = 0;
	pos_bit  = 0;
	maxsize_byte = 0;
	maxsize_bit  = 0;
	size_byte = 0;
	size_bit  = 0;
	char vobNr = 0;
	bool allocated = false;
	bool bits_written = false;
	bool buffer_error = false;
}

zCBuffer::~zCBuffer()
{
	if (allocated) // BUG! if Resize() on empty buffer -> memory leak
		delete buffer;
	buffer = nullptr;
}

void zCBuffer::Reset()
{
	pos_byte = 0;
	pos_bit = 0;
	size_byte = 0;
	size_bit = 0;
	bits_written = false;
}

void zCBuffer::Resize(size_t newsize_byte)
{
	if (newsize_byte >= maxSize)
	{
		if (newsize_byte < 32)
			newsize_byte = 32;
		auto newbuf = new zBYTE[newsize_byte]; // 168
		memset(newbuf, 0, newsize_byte);
		if (buffer)
			memcpy(newbuf, buffer, size);
		maxsize_byte = newsize_byte;
		maxsize_bit  = 8 * newsize_byte;
		if (buffer)
			delete buffer;
		buffer = newbuf;
	}
}

size_t zCBuffer::Write(const void* b, size_t len_byte)
{
	return (WriteBits(b, 8 * len_byte) + 7) / 8;
}


size_t zCBuffer::WriteBits(const void* b, size_t len_byte)
{
	if (buffer_error)
		return 0;

	if (pos_bit + len_bit > maxsize_bit)
	{
		auto newsize_bit = pos_bit + pos_bit / 2;
		if (newsize_bit < len_bit)
			newsize_bit += len_bit;
		Resize((newsize_bit + 7) / 8 + 1);
	}

	if (pos_bit & 7 || len_bit & 7)
	{
		for ( size_t i = 0; i < len_bit; ++i )
		{
			zBYTE* cursor;
			zBYTE  byte;
			if ((1 << (i & 7)) & b[i/8])
			{
				cursor = buffer[pos_bit/8];
				byte = (1 << (pos_bit & 7)) | *cursor;
			}
			else
			{
				cursor = buffer[pos_bit/8];
				byte = ~(1 << (pos_bit & 7)) & *cursor;
			}
			*cursor = byte;
			++pos_bit;
		}
	}
	else
	{
		memcpy(buffer + pos_bit, b, (len_bit + 7) / 8)
		pos_bit += len_bit;
	}
	pos_byte = (pos_bit + 7) / 8;
	if (pos_bit >= size_bit)
	{
		size_bit = pos_bit;
		size_byte = pos_byte;
	}
	return len_bit;
}

size_t zCBuffer::WriteBool(zBOOL value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteByte(zBYTE value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteWord(zWORD value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteDWord(zDWORD value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteReal(zREAL value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteInt(int value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteFloat(float value, int bits)
{
	return WriteBits(&value, bits);
}

size_t zCBuffer::WriteFloatPacked0_16(float value)
{
	zWORD val16 = (long long)(value * 65535.0);
	return (WriteBits(&val16, 0x10) + 7) >> 3;
}

size_t zCBuffer::WriteFloatPacked0_8(float value)
{
	zBYTE val8 = (long long)(value * 255.0);
	return (WriteBits(&val8, 0x8) + 7) >> 3;
}

size_t zCBuffer::WriteFloatPacked8_8(float value)
{
	zClamp(value, -127.0, 127.0);
	zWORD val8 = (long long)(value * 256.0);
	return (WriteBits(&val8, 0x10) + 7) >> 3;
}

size_t zCBuffer::WriteString(zSTRING value)
{
	auto b = value.Data();
	auto l = value.Length() + 1;
	return (WriteBits(b, l * 8) + 7) >> 3;
}

size_t zCBuffer::CompleteByte()
{
	zBYTE b = 0;
	auto nbits = GetSizeToNextByte();
	return WriteBits(&b, nbits);
}

size_t zCBuffer::Read(void* b, size_t len_byte)
{
	return (ReadBits(b, 8 * len_byte) + 7) / 8;
}

size_t zCBuffer::ReadBits(void* b, size_t len_bit)
{
	if (pos_bit & 7 || len_bit & 7)
	{
		for ( size_t i = 0; i < len_bit; ++i )
		{
			zBYTE* cursor;
			zBYTE  byte;
			if ((1 << (pos_bit & 7)) & buffer[pos_bit/8])
			{
				cursor = b[i/8];
				byte = (1 << (i & 7)) | *cursor;
			}
			else
			{
				cursor = b[i/8];
				byte = ~(1 << (i & 7)) & *cursor;
			}
			*cursor = byte;
			++pos_bit;
		}
	}
	else
	{
		memcpy(b, buffer + pos_bit, (len_bit + 7) / 8)
		pos_bit += len_bit;
	}
	pos_byte = (pos_bit + 7) / 8;
	return len_bit;
}

zBOOL zCBuffer::ReadBool(int bits)
{
	zBOOL b = 0;
	ReadBits(&b, bits);
	return b;
}

zBYTE zCBuffer::ReadByte(int bits)
{
	zBYTE b = 0;
	ReadBits(&b, bits);
	return b;
}

zWORD zCBuffer::ReadWord(int bits)
{
	zWORD b = 0;
	ReadBits(&b, bits);
	return b;
}

zDWORD zCBuffer::ReadDWord(int bits)
{
	zDWORD b = 0;
	ReadBits(&b, bits);
	return b;
}

zREAL zCBuffer::ReadReal(int bits)
{
	zREAL b = 0.0;
	ReadBits(&b, bits);
	return b;
}

int zCBuffer::ReadInt(int bits)
{
	int b = 0;
	ReadBits(&b, bits);
	return b;
}

float zCBuffer::ReadFloat(int bits)
{
	float b = 0.0;
	ReadBits(&b, bits);
	return b;
}

float zCBuffer::ReadFloatPacked0_16()
{
	zWORD b;
	ReadBits(&b, 0x10);
	return b / 65535.0;
}

float zCBuffer::ReadFloatPacked0_8()
{
	zBYTE b;
	ReadBits(&b, 0x8);
	return b / 255.0;
}

float zCBuffer::ReadFloatPacked8_8()
{
	zWORD b;
	ReadBits(&b, 0x10);
	return (signed short)(b) / 256.0;
}

int zCBuffer::ReadString(int bits)
{
	zSTRING ret;
	while (true)
	{
		zBYTE byte = ReadByte(8);
		if (!byte)
			break;
		ret += byte;
	}
	return ret;
}

void zCBuffer::SkipByte()
{
	auto diff = GetSizeToNextByte();
	if (diff > 0)
	{
		SetBitPosRel(diff);
	}
}

void zCBuffer::WriteObject(zCObject*& obj)
{
	++vobNr;

	zSTRING objectName = "vob" + vobNr;

	zCArchiver* arc = zarcFactory.CreateArchiverWrite(0, 1, 0);

	arc->writeObject(objectName, obj);

	zCBuffer* buf = arc->GetBuffer();

	size_t objectSize = buf->size_byte;

	WriteBits(objectSize, 32);
	WriteBits(buf->buffer, objectSize * 8);

	arc->Close();
	arc->Release();

	return objectSize;
}

void zCBuffer::ReadObject(zCObject*& obj)
{
	++vobNr;

	zSTRING objectName = "vob" + vobNr;
	size_t  objectSize = 0;
	ReadBits(&objectSize, 32);

	zCBuffer buf(objectSize);

	zCArchiver* arc = zarcFactory.CreateArchiverRead(0, &buf);

	arc->readObject(objectName, obj, 0);

	arc->Close();
	arc->Release();

	return objectSize;
}

void zCBuffer::CopyFrom(const zCBuffer& b)
{
	delete buffer;

	buffer = new zBYTE[b.size_byte]; // 653
	allocated = 1;
	memcpy(buffer, b.buffer, b.size_byte);

	mode = b.mode;
	maxsize_byte = b.maxsize_byte;
	maxsize_bit = b.maxsize_bit;
	bits_written = b.bits_written;
	buffer_error = b.buffer_error;
	pos_bit = b.pos_bit;
	pos_byte = b.pos_byte;
	size_bit = b.size_bit;
	size_byte = b.size_byte;
	vobNr = b.vobNr;
}

int byteFreqArray[256] = {};

void zCBuffer::ByteFeq_Reset()
{
	memset(byteFreqArray, 0, sizeof(byteFreqArray));
}

void zCBuffer::ByteFeq_Analyse()
{
	for (size_t i = 0; i < size_byte; ++i)
	{
		zBYTE byte = buffer[i];
		++byteFreqArray[byte];
	}
}

void zCBuffer::ByteFeq_Print() const
{
	zINFO_BEGIN(1, "B: BUF: Byte-Frequency"); //690

	int byteFreqArrayCopy[256];
	memcpy(byteFreqArrayCopy, byteFreqArray, sizeof(byteFreqArray));

	zSTRING outputStr;
	int colCcunter = 0;
	for (int i = 256; i; --i)
	{
		int count = -1;
		int byteIndex = 0;
		for (int i = 0; i < 256; ++i)
		{
			if ( count <= byteFreqArray[i] )
			{
				count = byteFreqArray[i];
				byteIndex = i;
			}
		}

		zSTRING str1 = zSTRING(byteIndex).Align(0, 3, spaceChar);
		zSTRING str2 = zSTRING(byteFreqArray[byteIndex]).Align(2, 4, spaceChar);

		outputStr = outputStr + str1 + "=" + str2 + "  ";

		byteFreqArray[byteIndex] = -1;

		++colCcunter;
		if (colCcunter > 8)
		{
			colCcunter = 0;
			zINFO("B: BUF: " + outputStr); //715
			outputStr.Clear();
		}
	}

	memcpy(byteFreqArray, byteFreqArrayCopy, sizeof(byteFreqArray));
}



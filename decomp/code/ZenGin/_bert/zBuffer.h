enum zTBufferMode {
	READ,
	WRITE,
	READWRITE
};

class zCBuffer {
public:
	zCBuffer();
	zCBuffer(size_t len_byte);
	zCBuffer(void* b, const size_t& len_byte);

	void Init();

	~zCBuffer();

	void SetMode(zTBufferMode mode)
	{
		this->mode = mode;
	}

	void Reset();
	void Resize(unsigned int newsize_byte);

	size_t Write(const void* b, size_t len_byte);
	size_t WriteBits(const void* b, size_t len_bit);

	size_t WriteBool(zBOOL value, int bits);
	size_t WriteByte(zBYTE value, int bits);
	size_t WriteDWord(zDWORD value, int bits);
	size_t WriteReal(zREAL value, int bits);

	size_t WriteInt(int value, int bits);
	size_t WriteFloat(float value, int bits);

	size_t WriteFloatPacked0_16(float value);
	size_t WriteFloatPacked0_8(float value);
	size_t WriteFloatPacked8_8(float value);

	size_t WriteString(zSTRING value);

	size_t CompleteByte();

	size_t Read(void* b, size_t len_byte);
	size_t ReadBits(void* b, size_t len_bit);

	zBOOL  ReadBool(int bits);
	zBYTE  ReadByte(int bits);
	zWORD  ReadWord(int bits);
	zDWORD ReadDWord(int bits);
	zREAL  ReadReal(int bits);

	int   ReadInt(int bits);
	float ReadFloat(int bits);

	float ReadFloatPacked0_16(); // not present in G2A
	float ReadFloatPacked0_8();
	float ReadFloatPacked8_8();

	zSTRING ReadString();

	void SetPos(const size_t& newpos_byte)
	{
		pos_bit = 8 * newpos_byte;
		pos_byte = newpos_byte;
	}

	void SetBitPos(const size_t& newpos_bit)
	{
		pos_bit = newpos_bit;
		pos_byte = (newpos_bit + 7) / 8;
	}

	void SetPosRel(const size_t& relmove_byte)
	{
		SetBitPosRel(8 * relmove_byte);
	}

	void SetBitPosRel(const size_t& relmove_bit)
	{
		SetPosBit(pos_bit + relmove_bit);
	}

	void SetPosBegin()
	{
		pos_bit = 0;
		pos_byte = 0;
	}

	void SetPosEnd()
	{
		SetBitPos(8 * size_byte);
	}

	int GetPos() const
	{
		return pos_byte;
	}

	int GetBitPos() const
	{
		return pos_bit;
	}

	int GetMaxSize() const
	{
		return maxsize_byte;
	}

	int GetBitMaxSize() const
	{
		return maxsize_bit;
	}

	int GetSize() const
	{
		return size_byte;
	}

	int GetBitSize() const
	{
		return size_bit;
	}

	int GetRestSize() const
	{
		return maxsize_byte - pos_byte;
	}

	int GetBitRestSize() const
	{
		return maxsize_bit - pos_bit;
	}

	char* GetBuffer()
	{
		return buffer;
	}

	char* GetCursor()
	{
		return &buffer[pos_byte];
	}

	void SkipByte();

	size_t GetSizeToNextByte() const
	{
		return pos_byte - (pos_bit + 7) / 8; // BUG!
	}

	bool HasError()
	{
		return buffer_error;
	}

	size_t WriteObject(zCObject*& obj);
	size_t ReadObject(zCObject*& obj);

	zCBuffer(const zCBuffer& b)
	{
		Init();
		CopyFrom(b);
	}

	zCBuffer& operator=(zCBuffer const& b)
	{
		if ( this != &b )
			CopyFrom(b);
		return *this;
	}

	void CopyFrom(const zCBuffer& b);

	void ByteFeq_Reset();
	void ByteFeq_Analyse() const;
	void ByteFeq_Print() const;

private:
	zTBufferMode mode;

	zBYTE* buffer;

	size_t pos_byte;
	size_t pos_bit;
	size_t maxsize_byte;
	size_t maxsize_bit;
	size_t size_byte;
	size_t size_bit;

	char vobNr;
	bool allocated;
	bool bits_written;
	bool buffer_error;
};

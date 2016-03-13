class zCChecksum16 {
public:
	void BeginChecksum()
	{
		this->checksum = 0;
	}

	void CalcBufferChecksum(uint8_t *buffer, size_t size)
	{
		checksum = GetBufferCRC16(buffer, size, checksum);
	}

	uint16_t EndChecksum()
	{
		return checksum;
	}

	static void GetBufferCRC16(uint8_t* buffer, size_t size, uint16_t& res);
private:
	uint16_t checksum;
}

void zCChecksum16::GetBufferCRC16(uint8_t* buffer, size_t size, uint16_t& res)
{
}

class zCChecksum {
public:
	void BeginChecksum()
	{
		hecksum = 0;
	}

	void CalcBufferChecksum(uint8_t *buffer, size_t size)
	{
		checksum = GetBufferCRC32(buffer, size, checksum);
	}

	uint32_t EndChecksum()
	{
		return checksum;
	}

	static void GetBufferCRC32(uint8_t* buffer, size_t size, uint32_t& res);
private:
	uint32_t checksum;
}

void zCChecksum::GetBufferCRC32(uint8_t* buffer, size_t size, uint32_t& res)
{
}

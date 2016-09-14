struct zCChecksum16 {
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

	static uint16_t GetBufferCRC16(uint8_t* buffer, size_t size, uint16_t init);

private:
	uint16_t checksum;
}

uint16_t zCChecksum16::GetBufferCRC16(uint8_t* buffer, size_t size, uint16_t init)
{
	if ( !buffer )
		return init;

	uint16_t result = init;
	if ( size >= 8 ) {
		size_t sz8 = size >> 3;
		for (; sz8; --sz8) {

			uint16_t v1 = result;
			uint16_t v2 = crc_table16[*(buffer++) ^ v1 & 0xFF]
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			v1 = (v1 >> 8) ^ v2;
			v2 = crc_table16[*(buffer++) ^ v1 & 0xFF];
			size -= 8;
			result = v1;
		}
	}

	for (; size; --size)
		result = (result >> 8) ^ crc_table16[*(buffer++) ^ result & 0xFF];
	return result;
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

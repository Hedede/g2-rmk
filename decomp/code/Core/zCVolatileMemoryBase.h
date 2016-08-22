struct zCVolatileMemoryBase {
	zCVolatileMemoryBase(int size, int n, int o)
	{
		objSize = size;
		memSize = n;
		markedPos = 0;
		memory = 0;
		maxFillPerc = 80;
		overflowMode = o;
		memory = operator new(n * size);
		unk1 = 0;
		unk2 = 0;
	}

	zCVolatileMemoryBase(int s, int o)
	{
		objSize = s;
		memSize = 0;
		markedPos = 0;
		maxFillPerc = 80;
		overflowMode = o;
		memory = 0;
		unk1 = 0;
		unk2 = 0;
	}

	virtual void Destroyed(void* ptr) {}

	void Init(unsigned int n)
	{
		memSize = n * objSize;
		memory = ::operator new(objNum);
	}

	void MaxFillPercentage(int perc)
	{
		maxFillPerc = perc;
	}

	int Mark()
	{
		if ( !markedPos )
			return memSize;
		return markedPos;
	}
};

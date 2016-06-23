struct zCFreeBlock {
	zCFreeBlock* next;
	void* dunno;
};

class zCMemPoolBase {
	static void SetMemDebug(char* className, char* fileName, int line)
	{
		s_className = className;
		s_fileName = fileName;
		s_line = line;
	}

	static void DisablePools(zBOOL b)
	{
		s_disablePools = b;
	}

protected:
	static zBOOL s_disablePools = 1;
	static char* s_className = 0;
	static char* s_fileName = 0;
	static int s_line = 0;
};

template<typename T>
struct zCMemPool : zCMemPoolBase {
	zCMemPool()
		: zCMemPoolBase(sizeof(T))
	{}
	zCMemPool(int blocksize, int prealloc)
		: zCMemPoolBase(sizeof(T), blocksize, prealloc)
	{}

	virtual zCMemPool() = default;

	void PoolAdd(T* mem, int num_objects, int free)
	{
		PoolAdd((void*)mem, num_objects, free);
	}
};

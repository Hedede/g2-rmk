struct zTMallocStats {
	int unk[10];
};

struct zCMalloc {
	zCMalloc() = default;
	~zCMalloc() = default;

	virtual void* Malloc(size_t size, char const *,char const *,int) = 0;
	virtual void* Malloc(size_t size) = 0;
	virtual void* Realloc(void* memory, size_t size) = 0;
	virtual void* Free(void* memory) = 0;


	virtual void GetStats(zTMallocStats& stats) {}
	virtual int DumpHeap(zTMallocDumpHeapMode) { return 0; }
	virtual void GetProcessStats(zTMallocProcessStats&) {}

	int CheckHeap() { return 1; }
	int CheckPtr(void*) { return 3; }

	virtual int Init(int) = 0;
	virtual int Shutdown() = 0;
};

struct zCMallocGeneric {
	zCMallocGeneric() = default;
	~zCMallocGeneric() = default;

	void* Malloc(size_t size, char const *,char const *,int) override
	{
		return malloc(size);
	}
	void* Malloc(size_t size) override
	{
		return malloc(size);
	}
	void* Realloc(void* memory, size_t size) override
	{
		return realloc(memory, size);
	}
	void* Free(void* memory) override
	{
		free(memory);
	}

	void GetStats(zTMallocStats& stats) override
	{
		stats = zTMallocStats{};
	}

	int Init(int) override { return 1; }
	int Shutdown() override { return 1; }
};

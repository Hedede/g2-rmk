class zCMalloc {
 virtual void ~zCMalloc(uint);
 virtual void GetStats(zTMallocStats &);
 virtual void GetProcessStats(zTMallocProcessStats &);
 virtual void DumpHeap(zTMallocDumpHeapMode);
	virtual void CheckHeap();
 virtual void CheckPtr(void *);
};

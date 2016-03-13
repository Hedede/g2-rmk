class zCMallocGeneric {
 virtual void ~zCMallocGeneric(uint);
 virtual void Malloc(uint,char const *,char const *,int);
 virtual void Malloc(uint);
 virtual void Realloc(void *,uint);
 virtual void Free(void	*);
	virtual void GetStats(zTMallocStats	&);
 virtual void Init(int);
 virtual void Shutdown();
};

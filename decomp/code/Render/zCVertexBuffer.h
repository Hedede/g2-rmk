class zCVertexBuffer : zCObject {
	~zCVertexBuffer();

	virtual void Create(ulong,ulong,ulong) { return 0; }
	virtual void Destroy() { return 0; }
	virtual void Lock(ulong) { return 0; }
	virtual void Unlock() { return 0; }
	virtual void IsLocked() { return 0; }
	virtual void Optimize() { return 0; }
	virtual void SetPrimitiveType(zTVBufferPrimitiveType) { return 0; }
	virtual void GetPrimitiveType() { return 0; }
	virtual void GetVertexFormat() { return 0; }
	virtual void GetVertexType() { return 0; }
	virtual void SetIndexList(zCArray<ushort> const &) { return 0; }
	virtual int SetIndexListSize(ulong) { return 0; }
	virtual int GetIndexListSize() { return 0; }
	virtual void* GetIndexListPtr() { return 0; }
};

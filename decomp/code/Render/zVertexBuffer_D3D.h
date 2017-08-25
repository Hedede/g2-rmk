class zCVertexBuffer_D3D : zCVertexBuffer {
	~zCVertexBuffer_D3D();

	virtual void Create(ulong,ulong,ulong);
	virtual void Destroy();
	virtual void Lock(ulong);
	virtual void Unlock();
	virtual void IsLocked();
	virtual void Optimize();
	virtual void SetPrimitiveType(zTVBufferPrimitiveType);
	virtual void GetPrimitiveType();
	virtual void GetVertexFormat();
	virtual void GetVertexType();
	virtual void SetIndexList(zCArray<ushort> const &);
	virtual void SetIndexListSize(ulong);
	virtual void GetIndexListSize();
	virtual void GetIndexListPtr();


	int unk[32];
};

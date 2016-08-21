class zCVertexBuffer {
 virtual void _GetClassDef();
 virtual void ~zCVertexBuffer(uint);
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
};

template <class T>
T const* zDYNAMIC_CAST(zCObject const* obj);

#define Z_OBJECT(T) \
	public: \
	void* operator new(size_t size) \
	{ \
		zCObject* ptr = (zCObject*)::operator new(size); \
		zCClassDef::ObjectCreated(ptr, &classDef); \
		return ptr; \
	} \
	void operator delete(void *ptr) \
	{ \
		zCClassDef::ObjectDeleted((zCObject*)ptr, &classDef); \
		::operator delete(ptr); \
	} \
	friend class zCClassDef; \
	friend T const* zDYNAMIC_CAST<T>(zCObject const*); \
	private: \
	static zCClassDef classDef; \
	static zCObject* _CreateNewInstance() \
	{ \
		return new T; \
	} \
	virtual zCClassDef* _GetClassDef() const \
	{ \
		return &classDef; \
	}

//NOTE: Bit field mask consts:     BitCount    FirstBit
const int zCObject_hashIndex = ((1 << 16) - 1) << 0;

class zCArchiver;

class zCObject {
	Z_OBJECT(zCObject);
public:
	static bool CheckInheritance(zCClassDef const* sought, zCClassDef const* def)
	{
		for (; def != nullptr; def = def->baseClassDef)
		{
			if (def == sought)
				return true;
		}
		return false;
	}

#if 0
	static bool CheckInheritance(zSTRING const& class1, zSTRING const& class2)
	{
		auto classDef1 = zCClassDef::GetClassDef(class1);
		auto classDef2 = zCClassDef::GetClassDef(class2);
		return CheckInheritance(classDef1, classDef2);
	}
#endif

	static bool CheckInheritance(zCObject const* o1, zCObject const* o2)
	{
		if ( o1 && o2 ) {
			return CheckInheritance(
			        o1->_GetClassDef(),
			        o2->_GetClassDef());
		}
		return 0;
	}

	virtual ~zCObject() = default;
	virtual void Archive(zCArchiver& arc) { }
	virtual void Unarchive(zCArchiver& arc) { }

	int Release()
	{
		--refCtr;
		if ( refCtr <= 0 )
			delete this;
		return refCtr;
	}

	int AddRef()
	{
		++refCtr;
	}

	int GetRefCtr() const
	{
		return refCtr;
	}

	zSTRING const& GetObjectName() const
	{
		return objectName;
	}

	void GetMemStats(int& numBytesTotal, int& numObjTotal, zCClassDef* parentClassDef);

	zCObject* zCObject::CreateCopy();
private:
	int  refCtr;
	int  hashIndex;
	int  hashNext;
	zSTRING objectName;
};

template <class T>
T const* zDYNAMIC_CAST(zCObject const* obj)
{
	if (!obj)
		return nullptr;

	zCClassDef* classDef = obj->_GetClassDef();
	while (classDef) {
		if (classDef == &T::classDef)
			return static_cast<T*>(obj);
		classDef = classDef->baseClassDef;
	}
	return nullptr;
}

inline void Release(zCObject*& obj)
{
	if (obj) {
		obj->Release();
		obj = 0;
	}
}

inline void AddRef(zCObject* obj)
{
	if (obj)
		obj->AddRef();
}

template<typename T>
inline void Set(T*& dst, T*& src)
{
	if (dst != src) {
		Release(dst);
		dst = src;
		AddRef(src);
	}
}

template<typename T>
inline void Delete(T*& ptr)
{
	if (ptr) {
		delete ptr;
		ptr = 0;
	}
}


zCObject* zCObject::CreateCopy()
{
	zCObject* copy = GetClassDef()->CreateNewInstance();
	if ( copy ) {
		zCArchiver* arcwrite = zarcFactory.CreateArchiverWrite(0, 0, 1, 0);
		arcwrite->WriteObject(this);
		zCBuffer* buffer = arcwrite->GetBuffer();
		if ( buffer ) {
			zCArchiver* arcread = zCArchiverFactory::CreateArchiverRead(buffer, 0);
			arcread->ReadObject(copy);
			arcread->Close();

			arcread->Release();

			buffer->SetMode((zTBufferMode)1);
		}

		arcwrite->Close();
		arcwrite->Release();
	}
	return copy;
}


void zCObject::GetMemStats(int& numBytesTotal, int& numObjTotal, zCClassDef* parentClassDef)
{
	zCArraySort__zCClassDef *v3; // ecx@3
	int v4; // ebx@3
	struct zCClassDef *v5; // eax@4

	if ( !parentClassDef ) {
		numBytesTotal = 0;
		numObjTotal = 0;
	}

	for (auto& cd : zCClassDef::classDefList) {
		if ( cd->baseClassDef == parentClassDef ) {
			numBytesTotal += cd->numLiving;
			numObjTotal   += cd->numLiving * cd->classSize;
			GetMemStats(numBytesTotal, numObjTotal, cd);
		}
	}
}

//NOTE: Bit field mask consts:     BitCount    FirstBit
const int zCObject_hashIndex = ((1 << 16) - 1) << 0;

class zCArchiver;

class zCObject {
	static zCClassDef classDef;
public:
	static bool CheckInheritance(zCClassDef const* sought, zCClassDef const* classDef)
	{
		while (classDef != nullptr)
		{
			if (classDef == sought)
				return true;
			classDef = classDef->baseClassDef;
		}
		return false;
	}



	static bool CheckInheritance(zCObject const* o1, zCObject const* o2);
	static bool CheckInheritance(zSTRING const& class1, zSTRING const& class2);

	zCObject() = default;
	virtual ~zCObject();

	virtual zCClassDef* _GetClassDef() const;
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

	zSTRING const& GetObjectName() const;
	int SetObjectName(zSTRING const& name);

	void GetMemStats(int& numBytesTotal, int& numObjTotal, zCClassDef* parentClassDef);

	zCObject* zCObject::CreateCopy();
private:
	int  refCtr;
	int  hashIndex;
	int  hashNext;
	zSTRING objectName;
};

#define zCLASS_DECLARATION(T) \
	public: \
	static zCClassDef classDef; \
	static zCObject* _CreateNewInstance() \
	{ \
		return new T; \
	} \
	zCClassDef* _GetClassDef() const override \
	{ \
		return &classDef; \
	} \
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
	}


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

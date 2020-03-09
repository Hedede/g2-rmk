class zCArchiver;
class zCObject;
class zCClassDef;

enum zTClassFlags {
	zCLASS_FLAG_SHARED_OBJECTS = 1 << 0,
	zCLASS_FLAG_TRANSIENT      = 1 << 1,
	zCLASS_FLAG_RESOURCE       = 1 << 2,
};


//--------------------------------------
// zCObject
//--------------------------------------
class zCObject {
	zCLASS_DECLARATION(zCObject);
public:
	static bool CheckInheritance(zCClassDef const* baseClassDef, zCClassDef const* classDef);
	static bool CheckInheritance(zCObject const* baseObject, zCObject const* subObject);
	static bool CheckInheritance(zSTRING const& baseClassName, zSTRING const& className);

	zCObject() = default;
	virtual ~zCObject();

	virtual void Archive(zCArchiver& arc) { }
	virtual void Unarchive(zCArchiver& arc) { }

	void Release()
	{
		if ( --refCtr <= 0 )
			delete this;
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

	zCObject* CreateCopy();

private:
	int  refCtr;
	int  hashIndex;
	int  hashNext;
	zSTRING objectName;
};

//--------------------------------------
// zCClassDef
//--------------------------------------
class zCClassDef {
	friend class zCObject;
	static zCArraySort<zCClassDef*>* classDefList;
	static zCClassDef* classDefSearchDummy;
public:
	using CreateObject = zCObject*(*)(void);

	zCClassDef() = default;
	zCClassDef(zSTRING const& className,
	           zSTRING const& baseName,
	           CreateObject create, size_t classSize,
	           int classFlags, short version,
	           zSTRING const& scriptName);

	~zCClassDef();

	static void ObjectCreated(zCObject *obj, zCClassDef* def);
	static void ObjectDeleted(zCObject *obj, zCClassDef* def);


	void Init();

	static zCClassDef* GetClassDef()
	{
		classDefSearchDummy->SetClassName(className);

		auto index = classDefList.Search(zCClassDef::classDefSearchDummy);
		if (index < 0)
			return nullptr;
		return classDefList[index];
	}

	uint16_t GetClassID()
	{
		return classDefList->Search(this);
	}

	zCObject* CreateNewInstance();

	static zCObject* CreateNewInstanceID(uint16_t id)
	{
		return classDefList->Get(id)->CreateNewInstance();
	}

	void ResetCreateNewInstance()
	{
		if ( createNewInstance )
			createNewInstance = createNewInstanceBackup;
	}

	void OverwriteCreateNewInstance(zCClassDef* other)
	{
		if (createNewInstance)
			createNewInstance = other->createNewInstance;
	}

	uint32_t GetHashIndex(zSTRING const& objname)
	{
		return zCChecksum::GetBufferCRC32(objname.Data(), objname.Size(), 0) & 0x3FF;
	}

	uint32_t GetHashIndex(zCObject* obj)
	{
		return GetHashIndex(obj->GetObjectName());
	}

private:
	void RemoveHashTable(zCObject* obj)
	{
		zCObject** ptr = &hashTable[obj->hashIndex];
		for (*ptr; ptr = &ptr->hashNext) {
			if (ptr == obj) {
				*ptr = obj->hashNext;
				obj->hashNext = nullptr;
				obj->hashIndex = -1;
			}
		}
	}

	void InsertHashTable(zCObject* obj)
	{
		auto hash = zCChecksum::GetBufferCRC32( obj->name.Data(), obj.name.Length(), 0 );
		auto index = hash & 0x3FF;

		obj->hashIndex = index;
		obj->hashNext = hashTable[index];
		hashTable[index] = obj;
	}

	zCObject* SearchHashTable(zSTRING const& name)
	{
		auto obj = hashTable[GetHashIndex(name)];
		for (; obj; obj = obj->hashNext) {
			if (obj->objectName == name)
				return obj;
		}
		return nullptr;
	}

	zSTRING className;
	zSTRING baseClassName;
	zSTRING scriptClassName;
	zCClassDef*  baseClassDef;

	zCObject* ( *createNewInstance) (void);
	zCObject* ( *createNewInstanceBackup) (void);

	uint32_t   classFlags = 0;
	uint32_t   classSize  = 0;

	int numLivingObjects = 0;
	int numCtorCalled    = 0;

	zCObject**    hashTable;

	zCArray<zCObject*> objectList;

	uint16_t archiveVersion    = 0;
	uint16_t archiveVersionSum = 0;
};

//--------------------------------------
// zCObjectFactory
//--------------------------------------
class zCObjectFactory : public zCObject {
	zCLASS_DECLARATION(zCObjectFactory)
public:
	virtual ~zCObjectFactory() = default;
	virtual class zCEventManager* CreateEventManager(zCVob *vob);
	virtual class zFILE* CreateZFile(zSTRING const& fileName);
	virtual class zCSession* CreateSession();
	virtual class zCCSManager* CreateCSManager();
	virtual class zCNetVobControl* CreateNetVobControl(zCVob *vob);
	virtual class zCGameInfo* CreateGameInfo();
	virtual class zCPlayerInfo* CreatePlayerInfo();
	virtual class zCWorld* CreateWorld();
	virtual class zCWayPoint* CreateWaypoint();
	virtual class zCWay* CreateWay();
};

//--------------------------------------
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

#define zRELEASE(obj) if (obj) { obj->Release(); obj = 0; }
#define zDELETE(obj)  if (obj) { delete obj; obj = 0; }

#define zCLASS_DECLARATION(T) \
	public: \
	static zCClassDef classDef; \
	static zCObject* _CreateNewInstance(); \
	virtual zCClassDef* _GetClassDef() const \
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

#define zCLASS_DEFINITION(Class, Base, Flags) \
zCClassDef Class::classDef(#Class, #Base, T::_CreateNewInstance, sizeof(T), 0, Flags, ""); \
static zCObject* _CreateNewInstance() { return new T; }

#define zCLASS_DEFINITION_SCRIPT(Class, Base, Flags, ScriptName) \
zCClassDef Class::classDef(#Class, #Base, T::_CreateNewInstance, sizeof(T), 0, Flags, ScriptName); \
static zCObject* _CreateNewInstance() { return new T; }

#define zABSTRACT_DEFINITION(Class, Base, Flags) \
zCClassDef Class::classDef(#Class, #Base, 0, sizeof(T), 0, Flags, "");

//--------------------------------------
// zCClassDef
//--------------------------------------

/* Für jede (von zCObject abgeleitete) Klasse gibt es
 * ein "Verwaltungsobjekt" vom Typ zCClassDef.
 * Dieses kapselt einige nützliche Informationen zu der
 * Gesamtheit der Objekte dieser Klasse.
 * Mit MEM_GetClassDef ( objPtr) kann das
 * zu einem Objekt gehörige zCClassDef Objekt bestimmt
 * werden. Das heißt für ein übergebenes Vob, bekäme
 * man zCClassDef für die Klasse zCVob.
 */

enum zTClassFlags {
	zCLASS_FLAG_SHARED_OBJECTS = 1<<0, //Mehrfach benutzt Objekte (wie Visuals zum Beispiel)
	zCLASS_FLAG_TRANSIENT      = 1<<1, //Flüchtig, soll nicht gespeichert werden.
	zCLASS_FLAG_RESOURCE       = 1<<2, //keine Ahnung / vermutlich irrelevant
};

class zCObject;

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

	~zCClassDef()
	{
		ReportLeaks();
		delete[] hashTable;

		classDefList.Remove(this);
		if (classDefList.IsEmpty()) {
			delete classDefList;
			delete classDefSearchDummy;
		}
	}

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
	zCClassDef*  baseClassDef;           //davon abgeleitet

	//Pointer auf klassenspezifische Funktion
	zCObject* ( *createNewInstance) (void);
	zCObject* ( *createNewInstanceBackup) (void);

	uint32_t   classFlags = 0; //siehe enum
	uint32_t   classSize  = 0; //Größe in Bytes

	int numLivingObjects = 0; //Anzahl Objekte von dieser Klasse
	int numCtorCalled    = 0; //Konstruktor wurde sooft aufgerufen

	//Hashtabelle der Größe 1024. Objekte sind mit zCObject.hashNext
	//verknüpft, falls mehrere auf den selben Wert hashen.
	zCObject**    hashTable;

	/*
	 * alle benannten
	 * (!) Objekte von genau
	 * (!) dieser Klasse
	 * (!) Ausrufezeichenanmerkungen:
	 * 1.) unbenannte sind nicht drin
	 * 2.) Objekte von Unterklassen sind nicht drin
	 * 3.) diese Eigenschaft kann sehr nützlich sein.
	 */
	zCArray<zCObject*> objectList;

	uint16_t archiveVersion    = 0; //vermutlich nutzlos
	uint16_t archiveVersionSum = 0; //vermutlich nutzlos
};

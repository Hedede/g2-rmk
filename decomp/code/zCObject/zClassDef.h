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

	uint16_t GetClassID()
	{
		return classDefList->Search(this);
	}

	zCObject* CreateNewInstance()
	{
		if ( createNewInstance )
			return createNewInstance();

		zFAULT("D: zObject(zCClassDef::CreateNewInstance): tried to create instance of abstrace class: " + className);
		return nullptr;
	}

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

//------------------------------------------------------------------------------
zCClassDef::zCClassDef(
            zSTRING const& className,
            zSTRING const& baseName,
            CreateObject create, size_t classSize,
            int classFlags, short version,
            zSTRING const& scriptName)
	: className(className), baseClassName(baseName),
	  scriptClassName(scriptName),
	  CreateNewInstance(create), CreateNewInstanceBackup(create),
	  classSize(classSize), classFlags(classFlags),
	  archiveVersion(version)
{
	if ( !classDefList )
	{
		classDefList = new zCArraySort<zCClassDef*>;
		classDefList->Compare = zCClassDef::CompareClassDefNames;
		classDefSearchDummy = new zCClassDef();
	}

	classDefSearchDummy->className = baseName;
	auto base = classDefList->Search(classDefSearchDummy);
	baseClassDef = base > 0 ? base : 0;

	if (!(classFlags & zCLASS_FLAG_SHARED_OBJECTS))
	{
		hashTable = (zCObject**) operator new(0x1000u);
		memset(hashTable, 0, 0x1000u);
	}

	classDefList->Insert(this);
}

void zCClassDef::Init()
{
	baseName = "";
	className = "";
	scriptName = "";
	baseClassDef = 0;
	create1 = 0;
	create2 = 0;
	numLiving = 0;
	numCtorCalled = 0;
	hashTable = 0;
	version = 0;
	versionSum = 0;
	classFlags = 0;
	classSize = 0;
}

void zCClassDef::ObjectCreated(zCObject *obj, zCClassDef *def)
{
	++def->numCtorCalled;
	++def->numLiving;
	if ( def->classFlags & zCLASS_FLAG_SHARED_OBJECTS )
	{
		objectList.Insert(obj);
	}

	if ( def == &zCObject::classDef )
	{
		zERROR::Fault("D: created instance of abstract 'zCObject' ?! => missing zCLASS_DECLARATION ?");
	}
}


void zCClassDef::EndStartup()
{
	if ( !classDefList )
		return;
	classDefList->QSort();

	for (auto cd : *classDefList) {
		classDefSearchDummy->className = cd->baseName;
		auto idx = classDefList->Search(classDefSearchDummy);
		if (idx > 0) {
			cd->baseClassDef = (*classDefList)[idx];
		} else {
			cd->baseClassDef = nullptr;
		}

		zCChecksum16 crc16;
		crc16.BeginChecksum();

		auto base = cd;
		do {
			crc16.CalcBufferChecksum(&base->version, 2);
			base = base->baseClassDef;
		} while (base);
		crc16.EndChecksum();
		cd->versionSum = crc16.GetChecksum();
	}

	zCClassDef::startupFinished = 1;
}


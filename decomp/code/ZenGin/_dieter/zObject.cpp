#include "ZenGin/_ulf/zEventMan.h"
#include "ZenGin/_ulf/zWaynet.h"
#include "ZenGin/_dieter/zWorld.h"
#include "ZenGin/_dieter/zContainer.h"

//------------------------------------------------------------------------------
zCLASS_DEFINITION(zCObjectFactory, NULL, 0); // 104
zCEventManager* zCObjectFactory::CreateEventManager(zCVob *vob) { return new zCEventManager(vob); }
zFILE* zCObjectFactory::CreateZFile(zSTRING const& fn) { return zFILE_VDFS(fn); }
zCSession* zCObjectFactory::CreateSession() { return new zCSession; }
zCCSManager* zCObjectFactory::CreateCSManager() { return new zCCSManager; }
zCNetVobControl* zCObjectFactory::CreateNetVobControl(zCVob *vob) { return new zCNetVobControl(vob); }
zCGameInfo* zCObjectFactory::CreateGameInfo() { return 0; }
zCPlayerInfo* zCObjectFactory::CreatePlayerInfo() { return 0; }
zCWorld* zCObjectFactory::CreateWorld() { return new zCWorld; }
zCWayPoint* zCObjectFactory::CreateWaypoint() { return zCWayPoint; }
zCWay* zCObjectFactory::CreateWay() { return new zCWay; }

//------------------------------------------------------------------------------
zCLASS_DEFINITION_ABSTRACT(zCObject, NULL, 0);

zCObject::~zCObject()
{
	zASSERT(refCtr<=0,  "D: OBJ: tried to delete zCObject with refCtr greater than 0 !"); // 131
	zASSERT(refCtr>=-1, "D: OBJ: deleting an already deleted zCObject ?!"); // 132

	refCtr = -999;
}

zCObject::_GetClassDef() const
{
	return &classDef;
}

bool zCObject::CheckInheritance(zCClassDef const* sought, zCClassDef const* classDef);
{
	while (classDef != nullptr)
	{
		if (classDef == sought)
			return true;
		classDef = classDef->baseClassDef;
	}
	return false;
}

bool zCObject::CheckInheritance(zSTRING const& class1, zSTRING const& class2)
{
	auto classDef1 = zCClassDef::GetClassDef(class1);
	auto classDef2 = zCClassDef::GetClassDef(class2);
	return CheckInheritance(classDef1, classDef2);
}

bool zCObject::CheckInheritance(zCObject const* baseObject, zCObject const* subObject)
{
	if ( baseObject && subObject ) {
		auto classDef1 = zCClassDef::GetClassDef(baseObject);
		auto classDef2 = zCClassDef::GetClassDef(subObject);
		return CheckInheritance(classDef1, classDef2);
	}
	return 0;
}

zCObject* zCObject::CreateNewInstance(const zSTRING& className)
{
	auto classDef = zCClassDef::GetClassDef(classDef); // was inlined
	if (!classDef)
		return nullptr;

	return classDef->CreateNewInstance();
}

zCObject* zCObject::CreateCopy()
{
	zCObject* copy = _GetClassDef()->CreateNewInstance();
	if ( copy )
	{
		zCArchiver* arcwrite = zarcFactory.CreateArchiverWrite(nullptr, 0, 1, 0);
		arcwrite->WriteObject(this);
		zCBuffer* buffer = arcwrite->GetBuffer();
		if ( buffer )
		{
			zCArchiver* arcread = zarcFactory.CreateArchiverRead(buffer, 0);
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

void zCObject::PrintClassHierarchy(int indent, zCClassDef* parentClassDef, int numObjTotal, int numBytesTotal)
{
	auto& classDefList = zCClassDef::classDefList;
	int numClasses = classDefList.GetNum()
	if ( !indent )
	{
		zINFO(1, "D: *** Enumerating class Hierarchy (#" + numClasses +  ") ***"); // 208
	}

	int currentLevel = indent;
	for (int i = numClasses; i > 0; --i)
	{
		auto classDef = classDefList[i];
		auto baseClassDef = classDef->baseClassDef;
		if ( baseClassDef == parentClassDef )
		{
			zSTRING name = baseClassDef ? baseClassDef->className : "NULL";
			zSTRING msg =  "D: " + Spaces( 4 * currentLevel ) + " - " + className->className;
			
			if (classDef->numLiving > 0)
			{
				msg += " [objs " + classDef->numLiving + "]";
			}

			if ( classDef->numCtorCalled > 0 )
			{
				msg += " [ctor " + classDef->numCtorCalled + "]";
			}

			if ( classDef->numLiving > 0 && classDef->classFlags & zCLASS_FLAG_SHARED_OBJECT )
			{
				msg += "[hash" + classDef->CalcHashTableSpread() + "]"; // was inlined
			}

			numObjTotal += classDef->numLiving;
			numBytesTotal += classDef->numLiving * v12->classSize;
			if ( name != classDef->baseName ) {
				msg += " (BASE_CLASS CORRUPT)";
				zWARNING(msg); // 240
			} else {
				zINFO(1, msg); //242
			}
			PrintClassHierarchy(indent + 1, classDef, numObjTotal, numBytesTotal);
		}
	}
	if ( !indent ) {
		int numObjTotal = 0;
		int numBytesTotal = 0;
		GetMemStats(numObjTotal, numBytesTotal, parentClassDef);

		zINFO(1, "D: ***********************************"); //252
		zINFO(1, "D: numObjTotal: " + numObjTotal + ", numBytesTotal: " + numBytesTotal); // 253
		zINFO(1, "D: ***********************************"); //254
	}
}

void zCObject::GetMemStats(int& numBytesTotal, int& numObjTotal, zCClassDef* parentClassDef)
{
	if ( !parentClassDef )
	{
		numBytesTotal = 0;
		numObjTotal = 0;
	}

	for (auto& cd : zCClassDef::classDefList)
	{
		if ( cd->baseClassDef == parentClassDef )
		{
			numBytesTotal += cd->numLiving;
			numObjTotal   += cd->numLiving * cd->classSize;
			GetMemStats(numBytesTotal, numObjTotal, cd);
		}
	}
}

zSTRING const& zCObject::GetObjectName() const
{
	return objectName;
}

int zCObject::SetObjectName(zSTRING const& name)
{
	if (_GetClassDef()->classFlags & zCLASS_FLAG_SHARED_OBJECTS == 0) {
		objectName = name;
		objectName.Upper();
		return 1;
	}

	if ( name ) {
		auto obj = _GetClassDef()->SearchHashTable( name ); // was inlined
		if ( obj ) {
			zWARNING("D: OBJ: SetObjectName() on shared object class '" + cd->className + "' failed: object with that name already exists! "); // 299
			return 0;
		}
	}

	// there was separate branch for each case with duplicate code
	// and jumps into each other

	if (objectName)
		_GetClassDef()->RemoveHashTable(this); // was inlined

	objectName = name;
	objectName.Upper();

	if (objectName)
		_GetClassDef()->InsertHashTable(this); // was inlined;

	return 1;
}

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

zCClassDef::~zCClassDef()
{
	ReportLeaks();
	delete[] hashTable;

	classDefList.Remove(this);
	if (classDefList.IsEmpty()) {
		delete classDefList;
		delete classDefSearchDummy;
	}
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

zCClassDef* zCClassDef::GetClassDef(zSTRING const& className)
{
	classDefSearchDummy->SetClassName(className);

	auto index = classDefList.Search(zCClassDef::classDefSearchDummy);
	if (index < 0)
		return nullptr;
	return classDefList[index];
}

zCClassDef* zCClassDef::GetClassDefTypeInsensitive(zSTRING const& className);
{
	zSTRING name = className;
	name.Upper();

	for (auto* classDef : classDefList)
	{
		zSTRING name2 = classDef->GetClassName();
		name2.Upper();
		if (name == name2)
			return classDef;
	}

	return nullptr;
}


zCObject* zCClassDef::CreateNewInstance();
{
	if ( createNewInstance )
		return createNewInstance();

	zFAULT("D: zObject(zCClassDef::CreateNewInstance): tried to create instance of abstrace class: " + className); // 538
	return nullptr;
}


void zCClassDef::ObjectCreated(zCObject *object, zCClassDef *objClassDef)
{
	++objClassDef->numCtorCalled;
	++objClassDef->numLiving;
	if ( objClassDef->classFlags & zCLASS_FLAG_SHARED_OBJECTS )
	{
		objectList.Insert(object);
	}

	if ( objClassDef == &zCObject::classDef )
	{
		zFAULT("D: created instance of abstract 'zCObject' ?! => missing zCLASS_DECLARATION ?"); // 640
	}
}

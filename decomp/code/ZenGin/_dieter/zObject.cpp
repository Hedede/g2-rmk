#include <zObject.h>
#include <zClassDef.h>

zOBJECT_CLASSDEF_ABSTRACT(zCObject, NULL, 0, 0);

zCObject::~zCObject()
{
	zASSERT(refCtr<=0, "D: OBJ: tried to delete zCObject with refCtr greater than 0 !"); // 131
	zASSERT(refCtr>=-1, "D: OBJ: deleting an already deleted zCObject ?!"); // 132

	refCtr = -999;
}

zCObject::_GetClassDef() const
{
	return &classDef;
}

bool zCObject::CheckInheritance(zSTRING const& class1, zSTRING const& class2)
{
	auto classDef1 = zCClassDef::GetClassDef(class1);
	auto classDef2 = zCClassDef::GetClassDef(class2);
	return CheckInheritance(classDef1, classDef2);
}

bool zCObject::CheckInheritance(zCObject const* o1, zCObject const* o2)
{
	if ( o1 && o2 ) {
		auto classDef1 = zCClassDef::GetClassDef(class1);
		auto classDef2 = zCClassDef::GetClassDef(class2);
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
	zCObject* copy = GetClassDef()->CreateNewInstance();
	if ( copy )
	{
		zCArchiver* arcwrite = zarcFactory.CreateArchiverWrite(0, 0, 1, 0);
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


zCObject* zCClassDef::CreateNewInstance();
{
	if ( createNewInstance )
		return createNewInstance();

	zFAULT("D: zObject(zCClassDef::CreateNewInstance): tried to create instance of abstrace class: " + className); // 538
	return nullptr;
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
		zFAULT("D: created instance of abstract 'zCObject' ?! => missing zCLASS_DECLARATION ?"); // 640
	}
}



#include <zObjectFactory.h>
zOBJECT_CLASSDEF(zCObjectFactory, zCObject, 0, 0);

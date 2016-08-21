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

	classDefList.Insert(this);
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


struct zCCollisionObjectDef {
	static zCArray<zCCollisionObjectDef*>* collObjClassList;

	zCCollisionObjectDef(zBOOL isVolatile, CtorFunc* CreateNewInstance);

	using CtorFunc = zCCollisionObject*();
	CtorFunc* CreateNewInstance;
	int isVolatile;
	int index;
};

zCCollisionObject::zCCollisionObject(zBOOL isVolatile, CtorFunc* createNewInstance)
{
	if (!collObjClassList)
		collObjClassList = new zCArray<zCCollisionObjectDef*>;

	this->isVolatile = isVolatile;
	this->CreateNewInstance = createNewInstance;

	index = collObjClassList->GetNumInList();
	collObjClassList->Insert(this);
}

struct zCCollisionObjectDef {
	using CtorFunc = zCCollisionObject*();

	CtorFunc* CreateNewInstance;
	int isVolatile;
	void *__next;
};

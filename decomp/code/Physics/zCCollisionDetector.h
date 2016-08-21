struct zCCollisionDetector {
	using ColDetFuncStatic = int(float,zCCollisionObject*,zMAT4 const&,zCCollisionObject*,zMAT4 const&,zCCollisionReport**);
	using ColDetFuncDynamic = int(zCCollisionObject*, zCCollisionObject*, zCCollisionReport**);

	zCCollisionDetector()
	{
		auto count = zCCollisionObjectDef::s_aoCollObjClassList->GetNumInList();
		staticTestFunc  = new ColDetFuncStatic*[count * count]{};
		dynamicTestFunc = new ColDetFuncStatic*[count * count]{};
	}

	virtual ~zCCollisionDetector()
	{
		delete[] staticTestFunc;
		delete[] dynamicTestFunc;
	}

	virtual void DetectCollisions(zCCollisionObject *,zCArray<zCCollisionObject *> const &,zCArray<zCCollisionReport *> &);
	virtual void RegisterStaticIntersectionTest(zCCollisionObjectDef *,zCCollisionObjectDef *, ColDetFuncStatic*);
	virtual void RegisterDynamicIntersectionTest(zCCollisionObjectDef *,zCCollisionObjectDef *, ColDetFuncDynamic*);

	virtual bool CanCollide(zCCollisionObjectDef* c1, zCCollisionObjectDef* c2)
	{
		auto idx = GetTestFuncIndex(c1, c2);
		return dynamicTestFunc[idx] || staticTestFunc[idx];
	}

private:
	static unsigned s_iCurrentCollReport;
	static zCCollisionReport s_aoCollReportList[16];

	static void S_ResetFreeCollReportList()
	{
		zCCollisionDetector::s_iCurrentCollReport = 0;
	}

	static zCCollisionReport* zCCollisionDetector::S_GetNextFreeCollReport()
	{
		if ( zCCollisionDetector::s_iCurrentCollReport >= 16 ) {
			zWARNING("D: CD: generic zCCollisionReport pool too small!");
			// 125, _dieter
		}

		return s_aoCollReportList[s_iCurrentCollReport++];
	}

	static int GetTestFuncIndex(zCCollisionObjectDef *collClass1, zCCollisionObjectDef *collClass2)
	{
		auto num = zCCollisionObjectDef::s_aoCollObjClassList->GetNumInList();
		return collClass1->index * num + collClass2->index;
	}

	ColDetFuncDynamic*& GetDynamicTestFunc(zCCollisionObjectDef* c1, zCCollisionObjectDef* c2)
	{
		return dynamicTestFunc[GetTestFuncIndex(c1, c2)];
	}

	ColDetFuncStatic*& GetStaticTestFunc(zCCollisionObjectDef* c1, zCCollisionObjectDef* c2)
	{
		return staticTestFunc[GetTestFuncIndex(c1, c2)];
	}

	void RegisterDynamicIntersectionTest(zCCollisionObjectDef* c1, zCCollisionObjectDef* c2, ColDetFuncDynamic* func)
	{
		GetDynamicTestFunc(c1, c2) = func;
	}

	void RegisterStaticIntersectionTest(zCCollisionObjectDef* c1, zCCollisionObjectDef* c2, ColDetFuncStatic* func)
	{
		GetStaticTestFunc(c1, c2) = func;
	}


private:
	ColDetFuncStatic** staticTestFunc;
	ColDetFuncDynamic** dynamicTestFunc;
};

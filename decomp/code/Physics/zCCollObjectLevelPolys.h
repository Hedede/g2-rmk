struct zCCollObjectLevelPolys : zCCollisionObject {
	zCCollisionObjectDef s_oCollObjClass;
	void GetCollObjClass() override
	{
		return &s_oCollObjClass;
	}

	zCCollObjectLevelPolys() = default;
	void ~zCCollObjectLevelPolys(uint) override = default;

	zCCollObjectLevelPolys* _CreateNewInstance()
	{
		return new zCCollObjectLevelPolys;
	}

private:
	zCArray<zCPolygon*> polys;
};

zCCollisionObjectDef* zCCollObjectLevelPolys::s_oCollObjClass{1, zCCollObjectLevelPolys::_CreateNewInstance};

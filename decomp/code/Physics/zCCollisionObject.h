struct zCCollisionObject {
	zCCollisionObjectDef s_oCollObjClass
	virtual zCCollisionObjectDef* GetCollObjClass()
	{
		return &s_oCollObjClass;
	}

	zCCollisionObject() = default;
	virtual ~zCCollisionObject();

	virtual void DetectCollisionsSelf(zCArray<zCCollisionObject*> const& collReportList, zCArray<zCCollisionReport*>&) {}
	virtual void CollisionResponse(zCArray<zCCollisionReport*> const &,int &) {}
	virtual float GetAdaptiveStepSize() { return 0.0f; }
	virtual int SuppressCollisionResponse() { return 0; }
	virtual void GetLargestBBox3DLocal(zTBBox3D& bbox)
	{
		bbox.mins = {-0.1, -0.1, -0.1};
		bbox.maxs = {0.1, 0.1, 0.1};
	}

	static zCCollisionObject* _CreateNewInstance() {
		return new zCCollisionObject;
	}

private:
	zMAT4 trafo_unk;
	zMAT4 trafoObjToWorld;
	zCVob *parent = nullptr;
	char flags;
	struct {
		char unk1 : 1;
		char unk2 : 2;
	} flags;
	/*
	char unk2[52];
	float floorY;
	float waterY;
	float ceilY;*/
};

zCCollisionObjectDef zCCollisionObject::s_oCollObjClass{1, zCCollisionObject::_CreateNewInstance};

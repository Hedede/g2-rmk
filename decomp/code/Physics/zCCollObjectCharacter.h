struct zCCollObjectCharacter : zCCollObjectBase {
	zCCollisionObjectDef s_oCollObjClass;
	zCCollisionObjectDef* GetCollObjClass() override
	{
		return &s_oCollObjClass;
	}

	struct zTSpatialState {
		float floorY;
		float waterY;
		float ceilY;
		int unk[5];
	};

	~zCCollObjectCharacter() override;
	void DetectCollisionsSelf(zCArray<zCCollisionObject*> const &,zCArray<zCCollisionReport *>	&) override;
	void CollisionResponse(zCArray<zCCollisionReport*> const &,int &) override;
	void GetAdaptiveStepSize() override;
	void GetLargestBBox3DLocal(zTBBox3D &) override;
	virtual void GetPrimitiveCC();
	virtual bool IsDead()
	{
		return false;
	}
	virtual bool DontLetHimComeNearer(zCVob const *)
	{
		return false;
	}
	virtual float TurnSpeed()
	{
		return 1.0f;
	}
	virtual void PushAround(zVEC3 const &) {}

	void IsMoveValid(zVEC3 const& transOld, zVEC3 const& transNew, zVEC3& transOut, zTSpatialState& spatialStateOut, zVEC& collisionNormalOut)
	{
		TestHardCollisions(transOld, transNew, transOut, spatialStateOut, collisionNormalOut);
	}

private:
	void PrintDebugInfos();

private:
	zCRayCache rayCache[6];
	void* unk2;
	zTSpatialState spatialState;
	char unk3[12];
	float slideRotDir;
	zVEC3 slideRotAxis;
	char unk4[52];
	zVEC3 collNormal;
};

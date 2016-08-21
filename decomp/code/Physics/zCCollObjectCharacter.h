struct zCCollObjectCharacter : zCCollObjectBase {
	virtual void GetCollObjClass();
	virtual ~zCCollObjectCharacter();
	virtual void DetectCollisionsSelf(zCArray<zCCollisionObject	*> const &,zCArray<zCCollisionReport *>	&);
	virtual void CollisionResponse(zCArray<zCCollisionReport	*> const &,int &);
	virtual void GetAdaptiveStepSize();
	virtual void GetLargestBBox3DLocal(zTBBox3D &);
	virtual void GetPrimitiveCC();
	virtual bool IsDead()
	{
		return false;
	}
	virtual bool DontLetHimComeNearer(zCVob const *)
	{
		return false;;
	}
	virtual void TurnSpeed();
	virtual void PushAround(zVEC3 const &) {}

	void IsMoveValid(zVEC3 const& transOld, zVEC3 const& transNew, zVEC3& transOut, zTSpatialState& spatialStateOut, zVEC& collisionNormalOut)
	{
		TestHardCollisions(transOld, transNew, transOut, spatialStateOut, collisionNormalOut);
	}

private:
	void PrintDebugInfos();
};

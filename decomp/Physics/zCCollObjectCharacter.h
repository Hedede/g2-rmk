class zCCollObjectCharacter {
 virtual void GetCollObjClass();
 virtual void ~zCCollObjectCharacter(uint);
 virtual void DetectCollisionsSelf(zCArray<zCCollisionObject	*> const &,zCArray<zCCollisionReport *>	&);
 virtual void CollisionResponse(zCArray<zCCollisionReport	*> const &,int &);
 virtual void GetAdaptiveStepSize();
 virtual void GetLargestBBox3DLocal(zTBBox3D &);
 virtual void GetPrimitiveCC();
 virtual void IsDead();
 virtual void DontLetHimComeNearer(zCVob const *);
 virtual void TurnSpeed();
	virtual void PushAround(zVEC3	const &);
};

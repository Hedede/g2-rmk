class zCCollisionObject {
public:
	virtual void GetCollObjClass();
	virtual ~zCCollisionObject();
	virtual void DetectCollisionsSelf(zCArray<zCCollisionObject	*> const &,zCArray<zCCollisionReport *>	&);
	virtual void CollisionResponse(zCArray<zCCollisionReport	*> const &,int &);
	virtual void GetAdaptiveStepSize();
	virtual void SuppressCollisionResponse();
	virtual void GetLargestBBox3DLocal(zTBBox3D &);
};

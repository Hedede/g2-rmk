class zCCollisionDetector {
public:
	virtual ~zCCollisionDetector();
	virtual void DetectCollisions(zCCollisionObject *,zCArray<zCCollisionObject *> const &,zCArray<zCCollisionReport *> &);
	virtual void RegisterStaticIntersectionTest(zCCollisionObjectDef *,zCCollisionObjectDef *,int (*)(float,zCCollisionObject *,zMAT4 const &,zCCollisionObject *,zMAT4 const &,zCCollisionReport * *));
	virtual void RegisterDynamicIntersectionTest(zCCollisionObjectDef *,zCCollisionObjectDef *,int (*)(zCCollisionObject *,zCCollisionObject *,zCCollisionReport * *));
	virtual void CanCollide(zCCollisionObjectDef *,zCCollisionObjectDef *);
};

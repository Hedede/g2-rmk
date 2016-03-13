class zCAIBase : public zCObject {
	Z_OBJECT(zCAIBase);
public:
	virtual ~zCAIBase();
	virtual void CanThisCollideWith(zCVob *);
	virtual void ReportCollisionToAI(zCCollisionReport const&);
	virtual void HasAIDetectedCollision();
	virtual void AICollisionResponseSelfDetected(zVEC3 const&, int&);
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *);
	virtual void HostVobAddedToWorld(zCVob *,zCWorld *);
	virtual void GetIsProjectile();
};

class zCAIBase : public zCObject {
	Z_OBJECT(zCAIBase);
public:
	virtual ~zCAIBase() = default;
	virtual void CanThisCollideWith(zCVob* vob) { return true; }
	virtual void ReportCollisionToAI(zCCollisionReport const&) {}
	virtual bool HasAIDetectedCollision() { return false; }
	virtual void AICollisionResponseSelfDetected(zVEC3 const&, int&) {}
	virtual void HostVobRemovedFromWorld(zCVob *,zCWorld *) {}
	virtual void HostVobAddedToWorld(zCVob *,zCWorld *) {}
	virtual bool GetIsProjectile() { return false; }
};

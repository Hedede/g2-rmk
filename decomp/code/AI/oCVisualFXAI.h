class oCVisualFXAI : public zCAIBase {
public:
	virtual ~oCVisualFXAI();
	virtual void DoAI(zCVob *,int &);
	virtual void ReportCollisionToAI(zCCollisionReport	const &);
	virtual void HostVobAddedToWorld(zCVob *,zCWorld *);
	virtual void GetIsProjectile();
};

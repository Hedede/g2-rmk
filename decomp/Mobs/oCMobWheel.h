class oCMobWheel : public oCMobInter {
	Z_OBJECT(oCMobWheel);
public:
	virtual ~oCMobWheel();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void Interact(oCNpc *,int,int,int,int,int);
};

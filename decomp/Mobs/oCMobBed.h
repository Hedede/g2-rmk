class oCMobBed : public oCMobInter {
	Z_OBJECT(oCMobBed);
public:
	virtual ~oCMobBed();
	virtual void GetScemeName();
	virtual void StartInteraction(oCNpc *);
	virtual void OnBeginStateChange(oCNpc*,int,int);
	virtual void OnEndStateChange(oCNpc *,int,int);
	virtual void SearchFreePosition(oCNpc *,float);
};

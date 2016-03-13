class oCCSTrigger : public zCCSTrigger {
	Z_OBJECT(oCCSTrigger);
public:
	virtual ~oCCSTrigger();
	virtual void TriggerTarget(zCVob *);
	virtual void UntriggerTarget(zCVob *);
};

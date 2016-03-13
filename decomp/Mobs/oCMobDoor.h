class oCMobDoor : public oCMobLockable {
	Z_OBJECT(oCMobDoor);
public:
	virtual ~oCMobDoor();
	virtual void GetScemeName();
	virtual void SearchFreePosition(oCNpc *,float);
	virtual void Open(oCNpc *);
	virtual void Close(oCNpc *);
private:
    zSTRING addName; // FRONT / BACK
};

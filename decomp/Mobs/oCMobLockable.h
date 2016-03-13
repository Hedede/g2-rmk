class oCMobLockable : public oCMobInter {
	Z_OBJECT(oCMobLockable);
private:
	static uint32_t scrambleLength = -1;
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~oCMobLockable();
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void Interact(oCNpc *,int,int,int,int,int);
	virtual void CanInteractWith(oCNpc *);
	virtual void CanChangeState(oCNpc *,int,int);
	virtual void OnEndStateChange(oCNpc *,int,int);
	virtual void SetLocked(int);
	virtual void SetKeyInstance(zSTRING const&);
	virtual void SetPickLockStr(zSTRING const&);
	virtual void Lock(oCNpc *);
	virtual void Unlock(oCNpc *,int);
	virtual void PickLock(oCNpc *,char);
private:
	uint32_t locked     : 1;
	uint32_t autoOpen   : 1;
	uint32_t pickLockNr : 30;

	zSTRING keyInstance;     //Schlüsselinstanzname
	zSTRING pickLockStr;     //linksrechtscombo
};

void oCMobLockable::Archive(zCArchiver& arc)
{
	oCMobInter::Archive(arc);

	arc.WriteGroupBegin("Lockable");

	arc.WriteBool("locked", locked);
	arc.WriteString("keyInstance", keyInstance);
	arc.WriteString("pickLockStr", pickLockStr);

	arc.WriteGroupEnd("Lockable");
}

void oCMobLockable::Unarchive(zCArchiver& arc)
{
	oCMobInter::Unarchive(arc);

	uint32_t tmp;
	arc.ReadBool("locked", tmp);
	locked = tmp;
	arc.ReadString("keyInstance", keyInstance);
	arc.ReadString("pickLockStr", pickLockStr);

	if ( !pickLockStr.IsEmpty() && ogame && ogame->inLoadSaveGame ) {
		if ( scrambleLength == -1 ) {
			scrambleLength = zoptions.ReadInt(zOPT_SEC_GAME, "pickLockScramble", 0);

			if ( scrambleLength <= 1 )
				scrambleLength = 0;
		}

		if (scrambleLength <= 1)
			return;

		int len = pickLockStr.Length();
		if ( len > 1 && len < scrambleLength ) {
			zSTRING tmp = pickLockStr;
			pickLockStr.Clear();
			for (int i = len; i > 1; --i) {
				int idx = rand() % i;

				pickLockStr += tmp[idx];
;
				tmp.Delete(idx, 1);
			}
		}
	}
}

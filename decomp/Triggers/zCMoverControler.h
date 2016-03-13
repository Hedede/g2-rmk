class zCMoverControler : public zCTriggerBase {
	Z_OBJECT(zCMoverControler);
public:
	virtual ~zCMoverControler();
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob*);
	virtual void OnUntouch(zCVob *);
};

void zCMoverControler::Archive(zCArchiver& arc)
{
	zCTriggerBase::Archive(arc);

	if ( arc.InProperties() ) {
		auto dummy = new zCEventMover(moverMessage);

		int num = dummy->MD_GetNumOfSubTypes();
		zSTRING enum_str;
		for (int i = 0; i < num; ++i) {
			if (i > 0)
				enum_str += ";";
			enum_str += dummy->MD_GetSubTypeString(i);
		}

		arc.WriteEnum("moverMessage", enum_str, moverMessage);

		// Memory leak in original!
		Release(dummy); //missing in original
	} else {
		arc.WriteEnum("moverMessage", "", moverMessage);
	}
	arc.WriteInt("gotoFixedKey", gotoFixedKey);
}

void zCMoverControler::Unarchive(zCArchiver& arc)
{
	zCTriggerBase::Unarchive(arc);
	moverMessage = arc.ReadEnum("moverMessage");
	arc.ReadInt("gotoFixedKey", gotoFixedKey);
}

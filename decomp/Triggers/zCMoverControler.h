class zCMoverControler : public zCTriggerBase {
	Z_OBJECT(zCMoverControler);
public:
	virtual ~zCMoverControler() = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	void OnTrigger(zCVob *,zCVob *) override;
	void OnUntrigger(zCVob *,zCVob *) override {}

	void OnTouch(zCVob*) override {}
	void OnUntouch(zCVob *) override {}

private:
	int moverMessage;
	int gotoFixedKey;
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

void zCMoverControler::OnTrigger(zCVob *other, struct zCVob *instigator)
{
	auto msg = new zCEventMover{moverMessage};
	msg->gotoFixedKey = gotoFixedKey;

	zCArray<zCVob*> list;

	homeWorld->SearchVobListByName(triggerTarget, &list);

	for (auto vob : list)
		vob->GetEM()->OnMessage(msg, this);
}

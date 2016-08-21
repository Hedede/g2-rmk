class zCMessageFilter : public zCTriggerBase {
	Z_OBJECT(zCMessageFilter);
public:
	virtual ~zCMessageFilter();

	virtual void Archive(zCArchiver& arc)
	{
		zCTriggerBase::Archive(arc);

		auto enumStr = "MT_NONE;MT_TRIGGER;MT_UNTRIGGER;MT_ENABLE;MT_DISABLE;MT_TOGGLE_ENABLED"
		arc.WriteEnum("onTrigger", enumStr, onTrigger);

		enumStr = "MT_NONE;MT_TRIGGER;MT_UNTRIGGER;MT_ENABLE;MT_DISABLE;MT_TOGGLE_ENABLED";
		arc.WriteEnum("onUntrigger", enumStr, onUntrigger);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCTriggerBase::Unarchive(arc);

		onTrigger   = arc.ReadEnum("onTrigger");
		onUntrigger = arc.ReadEnum("onUntrigger");
	}

	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
private:
	uint32_t onTrigger   : 4;
	uint32_t onUntrigger : 4;
};

void zCMessageFilter::Archive(zCArchiver& arc)


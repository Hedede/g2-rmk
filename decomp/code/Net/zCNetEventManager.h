class zCNetEventManager : public zCEventManager {
	Z_OBJECT(zCNetEventManager);
public:
	virtual void Archive(zCArchiver& arc)
	{
		zCEventManager::Archive(arc);

		if (arc.InSaveGame())
			arc.WriteObject(vobCtl);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEventManager::Unarchive(arc);
		if (arc.InSaveGame()) {
			GetNetVobControl(0);
			auto obj = arc.ReadObject(vobCtl);
			auto control = dynamic_cast<zCNetVobControl>(obj);
			if (!control)
				Release(vobCtl);
			vobCtl = control;
		}
	}

	virtual ~zCNetEventManager();
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void GetNetVobControl(int);
};

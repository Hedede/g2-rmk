class zCEventManager : public zCObject {
	Z_OBJECT(zCEventManager);
public:
	static int GetShowMessageCommunication()
	{
		return showMsgCommunication;
	}

	virtual void Archive(zCArchiver& arc)
	{
		arc.WriteBool("cleared", cleared);
		arc.WriteBool("active", isActive);
		arc.WriteObject("emCutscene", cutscene);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		arc.ReadBool("cleared", cleared);

		zBOOL tmp;
		arc.ReadBool("active", tmp);
		SetActive(tmp);

		auto obj = arc.ReadObject("emCutscene", 0);
		cutscene = dynamic_cast<zCCutscene>(obj);
	}

	virtual ~zCEventManager();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob	*);
	virtual void OnTouchLevel();
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void Clear();
	virtual void IsEmpty(int);
	virtual void GetCutsceneMode();
	virtual void SetCutscene(zCCutscene *);
	virtual void GetCutscene();
	virtual void IsRunning(zCEventMessage *);
	virtual void SetActive(int);
	virtual void GetNumMessages();
	virtual void GetEventMessage(int);
	virtual void GetActiveMessage();
	virtual void ShowList(int,int);
	virtual void GetNetVobControl(int);
	virtual void RemoveFromList(zCEventMessage *);
	virtual void InsertInList(zCEventMessage	*);
	virtual void ProcessMessageList();
	virtual void SendMessageToHost(zCEventMessage *,zCVob *,zCVob *);
	virtual void Delete(zCEventMessage *);
};

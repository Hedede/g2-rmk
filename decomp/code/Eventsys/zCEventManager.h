class zCEventManager : public zCObject {
	Z_OBJECT(zCEventManager);
private:
	static int showMsgCommunication;

public:
	static int GetShowMessageCommunication()
	{
		return showMsgCommunication;
	}

	static void GetShowMessageCommunication(int v)
	{
		showMsgCommunication = v;
	}

	void Archive(zCArchiver& arc) override
	{
		arc.WriteBool("cleared", cleared);
		arc.WriteBool("active", isActive);
		arc.WriteObject("emCutscene", cutscene);
	}

	void Unarchive(zCArchiver& arc) override
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
	virtual void OnTouchLevel() { hostVob->OnTouchLevel(); }
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);

	virtual void Clear();
	virtual void IsEmpty(int);
	virtual bool GetCutsceneMode() { return cutscene != 0; }
	virtual void SetCutscene(zCCutscene *);
	virtual zCCutscene* GetCutscene() { return cutscene; }
	virtual void IsRunning(zCEventMessage *);
	virtual void SetActive(int);

	virtual int GetNumMessages() { return msgList.GetNum(); }
	virtual zCEventMessage* GetEventMessage(int idx) { return msgList[idx]; }
	virtual void GetActiveMessage();

	virtual void ShowList(int,int);
	virtual zCNetVobControl* GetNetVobControl(int) { return nullptr; }
	virtual void RemoveFromList(zCEventMessage *);
	virtual void InsertInList(zCEventMessage	*);
	virtual void ProcessMessageList();
	virtual void SendMessageToHost(zCEventMessage *,zCVob *,zCVob *);
	virtual void Delete(zCEventMessage *);

	void Print_db(zSTRING const&, zCVob*) {}

private:
	int cleared;
	int isActive;
	zCCutscene *cutscene;
	zCArray<zCEventMessage*> msgList;
	zCVob* hostVob = nullptr;
};

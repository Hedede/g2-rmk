class zCEventManager {
public:
	virtual ~zCEventManager();
	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver &);
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void OnTouchLevel(void);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnMessage(zCEventMessage *,zCVob *);
	virtual void Clear(void);
	virtual void IsEmpty(int);
	virtual void GetCutsceneMode(void);
	virtual void SetCutscene(zCCutscene *);
	virtual void GetCutscene(void);
	virtual void IsRunning(zCEventMessage *);
	virtual void SetActive(int);
	virtual void GetNumMessages(void);
	virtual void GetEventMessage(int);
	virtual void GetActiveMessage(void);
	virtual void ShowList(int,int);
	virtual void GetNetVobControl(int);
	virtual void RemoveFromList(zCEventMessage *);
	virtual void InsertInList(zCEventMessage *);
	virtual void ProcessMessageList(void);
	virtual void SendMessageToHost(zCEventMessage *,zCVob *,zCVob *);

	virtual void Delete(zCEventMessage* msg)
	{
		if (msg->IsDeleteable()) {
			RemoveFromList(msg);
			msg->Release();
		}
	}
public:
	void* unk[19];
};

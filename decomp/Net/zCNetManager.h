class zCNetManager : public zCObject {
	Z_OBJECT(zCNetManager);
public:
	virtual ~zCNetManager();

	virtual void Process();
	virtual void CreateNetMessage(ushort const &,zCObject *);
	virtual void HandleNetMessage(zCNetMessage *,ushort const &,zCClassDef *,zCWorld *);
	virtual void ProcessMessages();
};

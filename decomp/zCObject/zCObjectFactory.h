class zCObjectFactory : public zCObject {
	Z_OBJECT(zCObjectFactory);
public:
	virtual ~zCObjectFactory();
	virtual void CreateEventManager(zCVob *);
	virtual void CreateZFile(zSTRING const &);
	virtual void CreateSession();
	virtual void CreateCSManager();
	virtual void CreateNetVobControl(zCVob *);
	virtual void CreateGameInfo();
	virtual void CreatePlayerInfo();
	virtual void CreateWorld();
	virtual void CreateWaypoint();
	virtual void CreateWay();
};

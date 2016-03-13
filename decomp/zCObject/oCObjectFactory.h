class oCObjectFactory : public zCObjectFactory {
	Z_OBJECT(oCObjectFactory);
public:
	virtual ~oCObjectFactory();
	virtual void CreateEventManager(zCVob *);
	virtual void CreateCSManager();
	virtual void CreateGameInfo();
	virtual void CreatePlayerInfo();
	virtual void CreateWorld();
	virtual void CreateWaypoint();
	virtual void CreateWay();
	virtual void CreateNpc(int);
	virtual void CreateItem(int);
};

class zCObjectFactory : public zCObject {
	Z_OBJECT(zCObjectFactory);
public:
	virtual void ~zCObjectFactory();
	virtual zCEventManager* CreateEventManager(zCVob *);
	virtual zFILE* CreateZFile(zSTRING const &);
	virtual zSession* CreateSession();
	virtual void CreateCSManager();
	virtual void CreateNetVobControl(zCVob *);
	virtual void CreateGameInfo();
	virtual void CreatePlayerInfo();
	virtual zCWorld* CreateWorld();
	virtual zCWaypoint* CreateWaypoint();
	virtual zCWay* CreateWay();
};

zCObjectFactory::eventManagers;

zCEventManager* zCObjectFactory::CreateEventManager(zCVob* vob)
{
	return new zCEventManager(vob);
}

zCEventManager* oCObjectFactory::CreateNewInstance(zCVob* vob)
{
	return new zCEventManager(vob);
}

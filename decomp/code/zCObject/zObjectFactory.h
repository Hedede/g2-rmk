class zCObjectFactory : public zCObject {
	Z_OBJECT(zCObjectFactory);
public:
	virtual ~zCObjectFactory() = default;
	virtual zCEventManager* CreateEventManager(zCVob *vob) { return new zCEventManager(vob); }
	virtual zFILE* CreateZFile(zSTRING const& fn) { return zFILE_VDFS(fn); }
	virtual zCSession* CreateSession() { return new zCSession; }
	virtual zCCSManager* CreateCSManager() { return new zCCSManager; }
	virtual zCNetVobControl* CreateNetVobControl(zCVob *vob) { return new zCNetVobControl(vob); }
	virtual zCGameInfo* CreateGameInfo() { return 0; }
	virtual zCPlayerInfo* CreatePlayerInfo() { return 0; }
	virtual zCWorld* CreateWorld() { return new zCWorld; }
	virtual zCWayPoint* CreateWaypoint() { return zCWayPoint; }
	virtual zCWay* CreateWay() { return new zCWay; }
};

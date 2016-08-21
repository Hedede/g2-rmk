class zTPortalInfo {
	zBYTE  visible;
	zBYTE  alpha;
};

struct oCPortalRoom {
	zSTRING& GetPortalName()
	{
		return portalName;
	}

private:
	zSTRING portalName;
	zSTRING ownerNpc;
	int ownerGuild;
};

class oCPortalRoomManager {
	zSTRING*        oldPlayerPortal;
	zSTRING*        curPlayerPortal;
	oCPortalRoom*   oldPlayerRoom;
	oCPortalRoom*   curPlayerRoom;

	zCArraySort<oCPortalRoom*> portals;
};

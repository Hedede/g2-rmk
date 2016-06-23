class zTPortalInfo {
	zBYTE  visible;
	zBYTE  alpha;
};

class oCPortalRoom {
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

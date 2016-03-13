class zTPortalInfo {
      zBYTE  visible;        //
      zBYTE  alpha;          //
};

class oCPortalRoom {
    zSTRING portalName;    //zSTRING 
    zSTRING ownerNpc;      //zSTRING 
    int ownerGuild;    //int           
};

class oCPortalRoomManager {
   zSTRING*        oldPlayerPortal;    //
   zSTRING*        curPlayerPortal;    //
   oCPortalRoom*   oldPlayerRoom;      //
   oCPortalRoom*   curPlayerRoom;      //

    zCArraySort <oCPortalRoom*> portals;
};

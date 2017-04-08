class zTPortalInfo {
	zBYTE  visible;
	zBYTE  alpha;
};

struct oCPortalRoom {
	oCPortalRoom() = default;
	oCPortalRoom(zSTRING const& name)
		: portalName{name}
	{}
	~oCPortalRoom() = default;

	void SavePortalData(zCArchiver& arc)
	{
		arc.WriteString("portalName", portalName);
		arc.WriteString("ownerNpc", ownerNpc);
		arc.WriteInt("ownerGuild", ownerGuild);
	}

	void LoadPortalData(zCArchiver& arc)
	{
		arc.ReadString("portalName", portalName);
		arc.ReadString("ownerNpc", ownerNpc);
		arc.ReadInt("ownerGuild", ownerGuild);
	}

	zSTRING& GetPortalName() const
	{
		return portalName;
	}

	int GetOwnerGuild() const
	{
		return ownerGuild;
	}

	oCNpc* GetOwnerNpc()
	{
		if (!ownerNpc)
			return nullptr;
		zCWorld* wld = 0;
		if (ogame)
			wld = ogame->GetWorld();
		if (!wld)
			return nullptr;
		auto owner = wld->SearchVobByName(ownerNpc);
		return zDYNAMIC_CAST<oCNpc>(owner);
	}

	bool IsPortalMyRoom(oCNpc* npc)
	{
		if (ownerNpc) {
			// wtf?! (comparing int with string)
			// It would be understandable if it was NPC ID
			// but instance ID can be different between game runs
			if (npc->GetInstance() == ownerNpc)
				return true;
		}

		if (ownerGuild < 0)
			return false;

		return ownerGuild == npc->GetTrueGuild();
	}

private:
	zSTRING portalName;
	zSTRING ownerNpc;
	int ownerGuild = -1;
};

class oCPortalRoomManager {
	zSTRING*        oldPlayerPortal;
	zSTRING*        curPlayerPortal;
	oCPortalRoom*   oldPlayerRoom;
	oCPortalRoom*   curPlayerRoom;

	zCArraySort<oCPortalRoom*> portals;
};

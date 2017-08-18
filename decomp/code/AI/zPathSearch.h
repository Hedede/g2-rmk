struct zSPathSearchState_Internal {
	zSPathSearchState_Internal() = default;
};

struct zCPathSearch {
	static zCPathSearch& GetSearch()
	{
		static zCPathSearch theSearch;
		return theSearch;
	}

	struct zSEvasionSearchOptions {
		int __flags;
		int __flags2;
		int unk1;
		float __flt;
		float __flt1;
		float __azi;
		float __elev;
		float __range;
		char aa[16];
	};
	

private:
	int FES_NoEvasionFound(zCArray<zSEvasionSearchDesc>& evasion);
	int FES_CamHitWall(zCArray<zSEvasionSearchDesc>& evasion) { return 1; }
	int FES_PlayerTargetInvisible(zCArray<zSEvasionSearchDesc>& evasion);

	int unk[6];
	float azi, elev, range;
	zMAT4 __mat;
	int unk0[52];
	zCAICamera *aicam;
	int unk1[3];
	int __collisionEnabled;
	zSEvasionSearchOptions __searchOptions;
	int more[24]; // also a struct?
	zCMovementTracker *movTracker;
	zCSphereCoordsCache __scoordCache;
};

//------------------------------------------------------------------------------
int AI_FindEvasionPanic(zSEvasionSearchDesc& desc)
{
	return zCPathSearch::GetSearch()->PanicSearch(desc);
}

int AI_FindEvasionSphere(zSEvasionSearchDesc& desc)
{
	return zCPathSearch::GetSearch()->SphereSearch(desc);
}

int AI_FindEvasionShoulderCam(zSEvasionSearchDesc& desc)
{
	return zCPathSearch::GetSearch()->ShoulderSearch(desc);
}




int zCPathSearch::FES_NoEvasionFound(zCArray<zSEvasionSearchDesc>& evasion)
{
	zSEvasionSearchDesc desc;
	desc.str = "__FP__";
	desc.searchFunc = AI_FindEvasionPanic;
	evasion.InsertEnd(desc);
	return 1;
}

int zCPathSearch::FES_PlayerTargetInvisible(zCArray<zSEvasionSearchDesc>& evasion)
{
	D_Print("FES Player Target Invisible", 2);
	__searchOptions.__flags |= 8u;
	return FES_PlayerInvisible_Rotated(evasion);
}

int zCPathSearch::FES_PlayerInvisible_Standing(zCArray<zSEvasionSearchDesc>& evasion)
{
	D_Print("FES Player Invisible (standing)", 2);
	__searchOptions.__flags |= 0x400u;
	return FES_PlayerTargetInvisible(evasion);
}

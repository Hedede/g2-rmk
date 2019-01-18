struct zSPathSearchState_Internal {
	zSPathSearchState_Internal() = default;
	
	char unk[120];
};

struct zSPathSearchState {
	float __aziStart;
	float __elevStart;
	float unk0;
	float __aziEnd;
	float __elevEnd;
	float __rotRange;
	float __aziDeg;
	float __fsin;
	float __fcos;
	char unk[76];
	zSTRING str1;
	zSTRING str;
};

struct zSPathSearchResult
{
	char unk[96];
};

struct zCPathSearch {
	static zCPathSearch& GetSearch()
	{
		static zCPathSearch theSearch;
		return theSearch;
	}

	struct zSPathSearchResult& GetReport()
	{
		static zSPathSearchResult theReport;
		return theReport;
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

	void SetAICamera(zCAICamera *camera) { aicam = camera; }
	

private:
	int FES_NoEvasionFound(zCArray<zSEvasionSearchDesc>& evasion);
	int FES_CamHitWall(zCArray<zSEvasionSearchDesc>& evasion) { return 1; }
	int FES_PlayerTargetInvisible(zCArray<zSEvasionSearchDesc>& evasion);

	int unk[6];
	float azi, elev, range;
	zMAT4 __mat1;
	int unk0[3];
	zSTRING __str1;
	zSTRING __str2;
	int unk1[9];
	zMAT4 __mat2;
	int unk2[3];
	zSTRING __str3;
	zSTRING __str4;
	int unk3;
	zCAICamera *aicam;
	zCVob *__camVob;
	zCVob *__target;
	zCWorld *__homeWorld;
	int __collisionEnabled;
	zSEvasionSearchOptions __searchOptions;
	int more[24]; // also a struct?
	zCMovementTracker *movTracker;
	zCSphereCoordsCache __scoordCache;
	zVEC3 __vec;
	int unk4[7];
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




//------------------------------------------------------------------------------
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



//------------------------------------------------------------------------------
//_carsten/zAiCamera_Core.cpp
void zCPathSearch::CamVobChanged()
{
	this->__camVob = aicam->__csVob;
	if ( __camVob ) {
		this->__homeWorld = __camVob->homeWorld;
		if ( !__homeWorld ) {
			zFAULT("C: zCPathSearch::CamVobChanged(). HomeWorld == 0"); //  180
		}
	}
}

void zCPathSearch::TargetChanged()
{
	this->__target = aicam->__target;
}

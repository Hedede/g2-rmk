class zCBspSector {
	zSTRING sectorName;

	zCArray<zCBspBase*> sectorNodes;

	zDWORD sectorIndex;		        //

	zCArray<zCPolygon*>	sectorPortals;

	zCArray<zTPortalInfo>	sectorPortalInfo;

	zTFrameCtr	  activated;          //
	zTFrameCtr	  rendered;           //
	zTBBox2D	 activePortal;
	zVEC3 sectorCenter[3];	//
	zBOOL8	 hasBigNoFade;		//
};

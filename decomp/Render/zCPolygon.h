//------------------------------------------------
//  zCPolygon
//------------------------------------------------

/* Bedeutung von zCPolygon.portalPoly:*/

enum zTPortalType   {
	zPORTAL_TYPE_NONE         = 0, 
	zPORTAL_TYPE_SMALL        = 1, 
	zPORTAL_TYPE_BIG          = 2, 
	zPORTAL_TYPE_BIG_NOFADE   = 3 
};

class zCVertFeature {
	zVEC3 vertNormal; 
	zCOLOR lightStat;    
	zCOLOR lightDyn;     
	zVALUE texu;         
	zVALUE texv;         
};

class zCPolygon {
public:
	static const int polyNumVert         = ((1 <<  8) - 1) <<  0;
	static const int portalPoly          = ((1 <<  2) - 1) <<  8;
	static const int occluder            = ((1 <<  1) - 1) << 10;
	static const int sectorPoly          = ((1 <<  1) - 1) << 11;
	static const int mustRelight         = ((1 <<  1) - 1) << 12;
	static const int portalIndoorOutdoor = ((1 <<  1) - 1) << 13;
	static const int ghostOccluder       = ((1 <<  1) - 1) << 14;
	static const int noDynLightNear      = ((1 <<  1) - 1) << 15;
	static const int sectorIndex         = ((1 << 16) - 1) << 16; //indoor only

	static void* morphedPolysSavedValuesList;

	zCVertex** vertex;                 // //array
	lastTimeDrawn;          //int

	zTPlane               polyPlane;

	zCMaterial*  material;               //
	zCLightMap*  lightmap;               //

	zCVertex      **    clipVert;               //
	zCVertFeature ** clipFeat;               //
	int numClipVert;            //int

	zCVertFeature ** feature;                ////array
	int bitfield;

};

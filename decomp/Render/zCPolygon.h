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

struct zCVertFeature {
	zVEC3 vertNormal;
	zCOLOR lightStat;
	zCOLOR lightDyn;
	zVALUE texu;
	zVALUE texv;
};

class zCPolygon {
	static void* morphedPolysSavedValuesList;
public:
	void SetMaterial(zCMaterial* newMat);

private:
	zCVertex** vertex;
	int lastTimeDrawn;

	zTPlane polyPlane;

	zCMaterial*  material;
	zCLightMap*  lightmap;

	zCVertex**      clipVert;
	zCVertFeature** clipFeat;
	int numClipVert;

	zCVertFeature** feature;     //array

	uint8_t polyNumVert;
	struct {
		uint8_t portalPoly  : 2;
		uint8_t occluder    : 1; // 4
		uint8_t sectorPoly  : 1; // 8
		uint8_t mustRelight : 1; // 0x10
		uint8_t portalIndoorOutdoor : 1; // 0x20
		uint8_t ghostOccluder       : 1; // 0x40
		//indoor only
		uint8_t noDynLightNear      : 1; // 0x80
	} flags;
	uint16_t sectorIndex;
};

void zCPolygon::SetMaterial(zCMaterial *newMat)
{
	if ( newMat != material ) {
		Release(material);
		AddRef(newMat);
		material = newMat;
	}
}

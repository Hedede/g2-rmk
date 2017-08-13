//------------------------------------------------
//  zCPolygon
//------------------------------------------------

/* Bedeutung von zCPolygon.portalPoly:*/
enum zTPortalType {
	zPORTAL_TYPE_NONE         = 0,
	zPORTAL_TYPE_SMALL        = 1,
	zPORTAL_TYPE_BIG          = 2,
	zPORTAL_TYPE_BIG_NOFADE   = 3 
};

struct zCVertFeature {
	zVEC3 vertNormal;
	zCOLOR lightStat;
	zCOLOR lightDyn;
	zVEC2 texuv;
};

class zCPolygon {
	static void* morphedPolysSavedValuesList;
public:
	static void S_InitMorph();
	static void S_EmptyMorphedVerts();
	static void PrepareRendering()

	void AddVertex(zCVertex *vert);

	void SetMapping(int vertNr, const zVEC2& texuv);
	void SetMaterial(zCMaterial* newMat);


	void Unclip_Occluder();

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

//------------------------------------------------------------------------------
void zCPolygon::S_InitMorph()
{
	zCPolygon::morphedVerticesList_Height = Compare_Verts;
	zCPolygon::morphedVerticesList_Wall = Compare_Verts;
	zCPolygon::morphedFeatureList = Compare_Features;
}

void zCPolygon::S_EmptyMorphedVerts()
{
	zCPolygon::morphedVerticesList_Height.Clear();
	zCPolygon::morphedVerticesList_Wall.Clear();
	zCPolygon::morphedFeatureList.Clear();
	zCPolygon::morphedPolysSavedValuesList.Clear();
}

void zCPolygon::PrepareRendering()
{
	zCPolygon::s_numClipVertScene = 0;
	zCPolygon::s_numVertListScene = 0;
	zCPolygon::s_numClipFeatScene = 0;
	zCPolygon::s_numFeatListScene = 0;
}

void zCPolygon::AddVertex(zCVertex *vert)
{
	AllocVerts(1);
	vertex[numVerts - 1] = vert;
}

void zCPolygon::SetMaterial(zCMaterial *newMat)
{
	if ( newMat != material ) {
		Release(material);
		AddRef(newMat);
		material = newMat;
	}
}

void zCPolygon::SetMapping(int vertNr, const zVEC2& texuv)
{
	feature[vertNr]->texuv = texuv;
}

void zCPolygon::Unclip_Occluder()
{
  clipVert = vertex;
  numClipVert = numVerts;
  clipFeat = feature;
}

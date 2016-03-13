class zCMesh : zCVisual {
private:
	int numPoly;
	int numVert;
	int numFeat;

	zCVertex**      vertList;
	zCPolygon**     polyList;
	zCVertFeature** featList;

	zCVertex*      vertArray;
	zCPolygon*     polyArray;
	zCVertFeature* featArray;

	zTBBox3D        bbox3D;

	zCOBBox3D        obbox3D;

	int masterFrameCtr;

	zCMesh*  next;
	zCMesh*  prev;

	zSTRING meshName;
	zBOOL  hasLightmaps;
	zBOOL  m_bUsesAlphaTesting;

	int    numVertAlloc;
	int    numPolyAlloc;
};

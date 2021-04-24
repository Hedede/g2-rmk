class zCPolyStrip : zCVisual {
	Z_OBJECT(zCPolyStrip);
public:
	zCPolyStrip();
	void ~zCPolyStrip()
	{
		FreeResources();
		Release(mat);
	}

	virtual void Render(zTRenderContext&);
	bool IsBBox3DLocal() override { return flags.isBBox3DLocal; };

	zTBBox3D GetBBox3D() override;
	void CalcBBox3d();

	zSTRING GetVisualName() override { return ""; }
	void SetVisualUsedBy(zCVob* vob) override
	{
		vob->flags1.0x20 = 1;
		owner = vob;
	}

	void AllocateResources(int numMaxSeg);
	virtual void FreeResources();

private:
	zCMaterial* material = nullptr;

	zCVertex*  verts = nullptr;
	zCPolygon* polys = nullptr;

	int numPoly = 0;
	int numVert = 0;

	zVEC3 *segmentCenter;
	float *segmentAlpha;

	float width;

	zCVob* connectedVob;

	zTBBox3D bbox;

	int __visible;
	int unk1;
	int __manual;
	int unk2;

	int visFirstSeg;
	int visLastSeg;
	int numSegments;
	float lastRemaider;
	float firstRemainder;

	float alphaFadeSpeed;
	float newAlphaFadeSpeed;

	float __defaultAlpha;

	int unk5;
	zVEC3 unk4;

	struct {
		unsigned isBBox3DLocal : 1;
	} flags;
};

//-------------------------------------------------------------------------------
// _Dieter/zPolyStrip.cpp
zCPolyStrip::zCPolyStrip()
{
}

zTBBox3D zCPolyStrip::GetBBox3D()
{
	if ( numSegments <= 0 ) {
		if ( connectedVob ) {
			bbox.mins = owner.GetPositionWorld();
			bbox.maxs = mins + 1.0;
		} else {
			bbox.mins = {0 ,0, 0};
			bbox.maxs = {1, 1, 1};
		}
	}
	return bbox;
}

void zCPolyStrip::CalcBBox3d()
{
	if (numSegments <= 0)
	{
		bbox.maxs = {0, 0, 0};
		bbox.mins = {0, 0, 0};
	}
	else
	{
		bbox.Init();

		int i = firstSeg;
		for ( int num = numSegments - 1; num > 0; --i) {
			bbox.addPoint( segmentCenter[i++] );
			if ( i >= numPoly )
				i = 0;
		}
	}
	if (connectedVob)
	{
		connectedVob->BeginMovement();
		connectedVob->SetPositionWorld(bbox.GetCenter());
		connectedVob->UpdateVisualDependencies(1);
		connectedVob->EndMovement(1);
	}
}

void zCPolyStrip::UpdateSegment(int segNr, const zVEC3& posUp, const zVEC3& posDown, const zVEC3& center)
{
	int n = segNr;
	if ( segNr == -1 )
		n = visLastSeg;
	verts[2 * n].Position     = posUp;
	verts[2 * n + 1].Position = posDown;
	segmentCenter[n] = center;
	segmentAlpha[n]  = __defaultAlpha;
}

void zCPolyStrip::UpdateSegmentAlpha(int segNr, float inNewAlpha)
{
	int n = segNr;
	if ( segNr == -1 )
		n = visLastSeg;
	segmentAlpha[n] = inNewAlpha;
}

void zCPolyStrip::AllocateResources(int numMaxSeg)
{
	numMaxSegments = numMaxSeg;
	numMaxVerts = 2 * numMaxSeg;
	polys = new zCPolygon[numMaxSeg]; // 173
	verts = new zCVertex[numMaxVerts]; // 174
	segmentCenter = new zVEC3[numMaxSeg]; // 175
	segmentAlpha  = new float[numMaxSeg]; // 176

	zCOLOR light = GFX_WHITE;

	for (int i = 0, i < numMaxSeg; ++i) {
		auto& poly = polys[i];
		int vi = i*2 + 3;
		poly.SetVertices(verts[vi - 3], verts[vi - 2], verts[vi], verts[vi - 1], 0);
		poly.SetMaterial(mat); // was inlined

		poly.feature[0]->texuv.x = 1.0;
		poly.feature[0]->texuv.y = 0.0;

		poly.feature[1]->texuv.x = 0.0;
		poly.feature[1]->texuv.y = 0.0;

		poly.feature[2]->texuv.x = 0.0;
		poly.feature[2]->texuv.y = 1.0;

		poly.feature[3]->texuv.x = 1.0;
		poly.feature[3]->texuv.y = 1.0;

		for (int i : range(4)) {
			poly.feature[i]->lightStat = light;
			poly.feature[i]->lightDyn = light;
		}

		segmentAlpha[i] = __defaultAlpha;
	}

	int vi = numMaxVerts - 1;
	polys[numMaxSegments - 1].SetVertices( verts[vi - 2], verts[vi - 1], verts[1], verts[0], 0 );
}

void zCPolyStrip::EmptyEffect()
{
	firstRemainder = 1.0;   // 1.0
	lastRemaider   = 0;
	numSegments = 0;
	visFirstSeg = 0;
	visLastSeg = 0;
	__defaultAlpha = 255.0;
}

void zCPolyStrip::FreeResources()
{
	delete[] verts;
	delete[] polys;

	delete segmentCenter;
	delete segmentAlpha;

	polys = 0;
	verts = 0;
	segmentCenter = 0;
	segmentAlpha  = 0;

	numMaxSegments = 0;
	numMaxVerts    = 0;

	EmptyEffect();
}

void zCPolyStrip::SetVisibleSegments(float visibleFirst, float visibleLast)
{
	float first = visibleFirst * numMaxSegments;
	float last  = visibleLast  * numMaxSegments;

	visFirstSeg = floor(first) % numMaxSegments;
	visLastSeg  = ceil(last)   % numMaxSegments;

	firstRemainder =       first - floor(first);
	lastRemaider   = 1.0 - (last - floor(last));

	int delta = visLastSeg - visFirstSeg;
	if ( visLastSeg <= visFirstSeg )
		numSegments = delta + numMaxSegments + 1;
	else
		numSegments = delta + 1;

	segmentAlpha[visLastSeg] = __defaultAlpha;
}

void zCPolyStrip::AddSegment(zVEC3 const& centerPoint)
{
	if ( numSegments < numMaxSegments ) {
		int visLastSeg = (numSegments + visFirstSeg) % numMaxSegments;
		segmentCenter[visLastSeg] = *centerPoint;
		bbox.AddPoint(centerPoint);
	}
}

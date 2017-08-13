// UNUSED CLASS (since g1 demo)
//------------------------------------------------------------------------------
// not sure about base class, size matches,
// but vtable is completely missing
struct zCLightning : zCVisual {
	struct zCBolt : zCPolyStrip {
		zCBolt() = default;
		~zCBolt() = default;

		void Render(zTRenderContext& ctx) override;
		void FreeResources() override;
		void Generate_R(const zVEC3& startPos, const zVEC3& endPos, float prob);

	private:
		zCArray<zCBolt*> branches;
	};

	static void SetProjectionSphere(zTBSphere3D const& sphere);
	void SetVisualUsedBy(zCVob* vob);
	void Generate(const zVEC3& startPos, const zVEC3& endPos);
	void Render(zTRenderContext& ctx)
	{
		bolt.Render();
	}

private:
	zCBolt bolt; // not a pointer
};

//------------------------------------------------------------------------------
zVEC3 boltGlobalEndPos;
zTBSphere3D boltGlobalSphere;

void zCLightning::Generate(const zVEC3& startPos, const zVEC3& endPos)
{
	boltGlobalEndPos = endPos;
	bolt.FreeResources();
	bolt.mat.color[3] = 200;
	bolt.__width = 30.0;
	bolt.Generate_R(startPos, endPos, 1.0);
}

void zCLightning::SetProjectionSphere(zTBSphere3D const& sphere)
{
	boltGlobalSphere = sphere;
}

void zCLightnig::SetVisualUsedBy(zCVob *vob)
{
	vob->flags1.0b100000 = 1;
	vob->SetCollDetStat(0);
	vob->SetCollDetStat(0);
}

void zCLightning::zCBolt::Render(zTRenderContext& ctx)
{
	zCPolyStrip::Render(ctx);
	for (auto* bolt : branches)
		bolt->Render(ctx);
}

void zCLightning::zCBolt::FreeResources() override;
{
	zCPolyStrip::FreeResources();
	branches.DeleteList();
}

void zCLightning::zCBolt::Generate_R(const zVEC3& startPos, const zVEC3& endPos, float prob)
{
	float num = (rand() / RAND_MAX + 1.0) * 3.0;
	if ( num < prob )
		return;

	zVEC3 dir = endPos - startPos;
	float distApprox = dir.LengthApprox();
	int bit = zGetHighestBit( int(distApprox / 62.5) );
	AllocateResources( 1 << bit + 1 );
	GenerateCPLightning0(startPos, endPos, 0.6);

	int roll = rand() % 5;
	if ( !roll )
		return;

	branches.AllocAbs( roll );

	for (int i = 0; i < roll; ++i) {
		subBolt = new zCBolt();
		int idx = visFirstSeg + rand() % (numSegments / 2);
		zVEC3 center = segmentCenter[idx % numSegments];

		zVEC3 rvec{
			zRandF2(),
			RANDOM(-1.0, -0.5), // -((rand() / RAND_MAX) + 1.0) / 2
			zRandF2()
		};

		rvec.NormalizeApprox();
		rvec *= RANDOM(0.3, 0.6);
		rvec *= distApprox;
		rvec += center;

		rvec.y = zMax( rvec.y, boltGlobalEndPos.y );
		subBolt->mat->color[3] = mat->color[3] * 0.4;
		subBolt->__width = __width * 0.6;
		subBolt->Generate_R(center, rvec, prob + 1.0);
		branches.InsertEnd(subBolt);
	}
}

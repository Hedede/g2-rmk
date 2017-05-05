class zCParticleFX : public zCVisual {
	Z_OBJECT(zCParticleFX);
public:
	static zSParticle* s_globPartList;
	static unsigned s_globNumPart;
	static InitParticleFX();

	virtual ~zCParticleFX();

	void Render(zTRenderContext &);
	bool IsBBox3DLocal() override
	{
		return false
	}
	void GetBBox3D() override;
	zSTRING GetVisualName() override
	{
		if (emitter->pfxName.Length() > 0)
			return emitter->pfxName;
		return "";
	}
	bool GetVisualDied() override
	{
		return (flags >> 2) & 1;
	}
	void SetVisualUsedBy(zCVob* vob) override
	{
		vob->flags1.ignoredByTraceRay = 1;

		usedBy = vob;

		bbox.mins = vob->GetPositionWorld() - 1.0;
		bbox.maxs = vob->GetPositionWorld() + 2.0;
	}
	int GetRenderSortKey() override
	{
		return emitter;
	}
	void HostVobRemovedFromWorld(zCVob *,zCWorld *) override {}
	void HostVobAddedToWorld(zCVob*, zCWorld* world) override
	{
		world->RegisterPerFrameCallback(&s_pfxPerFrameCallback);
	}

	zSTRING* GetFileExtension(int idx) override
	{
		static zSTRING ext = ".PFX";

		if (idx != 0)
			return 0;
		return &ext;
	}

	void LoadVisualVirtual(zSTRING const& visualName) override
	{
		Load(visualName);
	}

	void StopEmitterOutput()
	{
		flags |= 8;
	}

private:
	static double PartRand()
	{
		return (rand() - 16383.5) / 16383.5;
	}

	struct zCStaticPfxList {
		// head and tail?
		zCParticleFX *pfx1;
		zCParticleFX *pfx2;
		int count;
	};

	void InitEmitterVars()
	{
		memset(emitter_vars, 0, sizeof(emitter_vars));
	}

private:
	zSParticle *particles;

	int emitter_num_arr1;
	char emitter_vars[24];
	zCParticleEmitter *emitter;

	zTBBox3D bbox;
	zCVob* usedBy;
	int unk2;

	char flags;

	zCParticleFX *__next; // maybe other way around?
	zCParticleFX *__prev;

	int unk3;
	float __timeStamp;
	char unk4[8];
	zCQuadMark* __quadMark;

	char unk5[29];
};

void zCParticleFX::InitParticleFX()
{
	if ( zCParticleFX::s_globPartList )
		delete[] s_globPartList;

	s_globPartList = new zSParticle[10000];
	s_globNumPart = 10000;

	for (unsigned i = 0; i < s_globNumPart; ++i)
		s_globNumPart[i].next = &s_globPartList[i+1];
	s_globPartList[s_globNumPart - 1].next = 0;

	if ( !zCParticleFX::s_partMeshTri ) {
		s_partMeshTri = zCMesh::CreateTriMesh();

		auto mat = new zCMaterial("Z_PART_TRI");

		s_partMeshTri->SetMaterial(mat);
		s_partMeshTri->SetStaticLight(GFX_CYAN);
		s_partMeshTri->ResetLightDynToLightStat();

		mat->alphaFunc = 2;
		mat->color = GFX_CYAN;
		mat->Release();

		s_partMeshQuad = zCMesh::CreateQuadMesh(0);

		mat = new zCMaterial("Z_PART_QUAD");

		s_partMeshQuad->SetMaterial(GFX_CYAN);
		s_partMeshQuad->SetStaticLight(GFX_CYAN);
		s_partMeshQuad->ResetLightDynToLightStat();

		mat->alphaFunc = 2;
		mat->color = GFX_CYAN;
		mat->Release();

	}

	s_emitterPresetList.Compare = CompareEmitterNames;
	ParseParticleFXScript();
}

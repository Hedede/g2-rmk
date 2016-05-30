class zCParticleFX : public zCVisual {
	Z_OBJECT(zCParticleFX);
public:
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
	void InitEmitterVars()
	{
		memset(emitter_vars, 0, sizeof(emitter_vars));
	}

private:
	int unk0;
	char emitter_vars[28];
	zCParticleEmitter *emitter;

	zTBBox3D bbox;
	zCVob* usedBy;
	int unk2;

	char flags;

	char unk3[60];
};

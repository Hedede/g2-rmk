class zCMorphMesh : public zCVisualAnimate{
	Z_OBJECT(zCMorphMesh);
public:
	virtual ~zCMorphMesh();
	virtual void Render(zTRenderContext	&);
	bool IsBBox3DLocal() override
	{
		return true;
	}
	virtual void GetBBox3D();
	virtual zCOBBox3D* GetOBBox3D();
	virtual void GetVisualName();
	virtual void GetRenderSortKey();
	bool CanTraceRay() override
	{
		return true;
	}
	virtual void TraceRay(zVEC3 const &,zVEC3 const &,int,zTTraceRayReport &);
	virtual void GetFileExtension(int);
	bool GetAlphaTestingEnabled() override
	{
		return texAniState.alphaTestingEnabled;
	}
	void SetAlphaTestingEnabled(int b) override
	{
		texAniState.alphaTestingEnabled = b;
	}
	virtual void LoadVisualVirtual(zSTRING const	&);
	virtual void StartAnimation(zSTRING const &);
	virtual void StopAnimation(zSTRING const &);
	virtual void IsAnimationActive(zSTRING const &);
	virtual void GetAnyAnimation();

	void SetTextureVar(int channel, int varNr, zSTRING& texNamePart)
	{
		texAniState.SetChannelVariation(channel, varNr, texNamePart);
	}

private:
	int unk[3];
	zCModelTexAniState texAniState;
	char unk1[12];
	zCArraySort< unk > unk2;
};

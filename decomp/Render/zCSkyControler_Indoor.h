class zCSkyControler_Indoor : public zCSkyControler_Mid {
	Z_OBJECT(zCSkyControler_Indoor);
public:
	virtual ~zCSkyControler_Indoor();
	virtual void SetTime(float);
	virtual void GetTime();
	virtual void ResetTime();
	virtual void SetFarZ(float);
	virtual void GetFarZ();
	virtual void SetFarZScalability(float);
	virtual void GetFarZScalability();
	virtual void SetBackgroundColor(zCOLOR);
	virtual void GetBackgroundColor();
	virtual void GetBackgroundColorDef();
	virtual void SetOverrideColor(zVEC3);
	virtual void SetOverrideColorFlag(int);
	virtual void UpdateWorldDependencies();
	virtual void RenderSkyPre();
	virtual void RenderSkyPost(int);
};

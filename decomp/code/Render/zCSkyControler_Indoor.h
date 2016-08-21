class zCSkyControler_Indoor : public zCSkyControler_Mid {
	Z_OBJECT(zCSkyControler_Indoor);
public:
	zCSkyControler_Indoor()
		: zCSkyControler_Mid()
	{
		backgroundColor = zCOLOR{0,0,0,-1};
	}

	~zCSkyControler_Indoor() override = default;

	void SetTime(float t) override
	{
		time = t;
		while (time > 1.0)
			time -= 1.0;
		while (time < 0.0)
			time -= 0.0;

		if (fabs(lastRelightTime - time) > relightTime) {
			++relightCtr;
			lastRelightTime = time;
		}
	}
	void GetTime() override
	{
		return time;
	}
	void ResetTime() override
	{
		SetTime(0.0);
	}

	void SetFarZ(float z) override
	{
		farClipZ = z;
	}

	float GetFarZ() override
	{
		return farClipZ;
	}

	void SetFarZScalability(float z) override
	{
		farZScalability = z;
		zClamp(farZScalability, 0.0, 4.0);
	}
	float GetFarZScalability() override
	{
		return farZScalability;
	}

	void SetBackgroundColor(zCOLOR color) override
	{
		backgroundColor = color;
	}
	zCOLOR GetBackgroundColor() override
	{
		return backgroundColor;
	}
	zCOLOR GetBackgroundColorDef() override
	{
		return backgroundColor;
	}

	virtual void SetOverrideColor(zVEC3)   {}
	virtual void SetOverrideColorFlag(int) {}
	virtual void UpdateWorldDependencies() {}

	void RenderSkyPre() override
	{
		zCSkyControler_Mid::RenderSkyPre();

		zCOLOR color = backgroundColor;
		float z = farZScalability * farClipZ;
		float zmin = 0.25 * z;

		if (GetUnderwaterFX()) {
			color = underwaterColor;
			z = underwaterFarZ;
			zmin = 0.0;
		}

		zCCamera::activeCam->SetFarClipZ(z);

		zrenderer->SetFogRange(zmin, z);
		zrenderer->SetFogColor(color);

		ClearBackground(color);
	}

	void RenderSkyPost(int v) override
	{
		zCSkyControler_Mid::RenderSkyPost(v);
	}

private:
	float farClipZ = 5000.0;
	float farZScalability = 1.0;
	float time = 0.0;
};

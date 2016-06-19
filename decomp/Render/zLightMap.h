class zCLightMap : public zCObject {
	Z_OBJECT(zCLightMap);
public:
	zCLightMap() = default;
	virtual ~zCLightMap()
	{
		Release(texture);
	}

	void SetTexture(zCTexture* texture)
	{
		Set(this->texture, texture);
	}

	void CalcLightmapOriginUpRight(zVEC3 const& origin, zVEC3 const& up, zVEC3 const& right)
	{
		float fac = 1.0 / (right.Length2() * up.Length2() * up.Length2();
		lmRight   = right * fac;
		lmUp      = up    * (1.0 / up.Length2());
		lmOrigin  = origin;
	}

private:
	zVEC3 lmOrigin;
	zVEC3 lmUp;
	zVEC3 lmRight;
	zCTexture* texture = nullptr;
};

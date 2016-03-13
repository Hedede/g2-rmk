class zCSkyControler_Mid : public zCSkyControler {
	Z_OBJECT(zCSkyControler_Mid);
public:
	virtual ~zCSkyControler_Mid();
	virtual void SetUnderwaterFX(int);
	virtual void GetUnderwaterFX();
	virtual void RenderSkyPre();
	virtual void RenderSkyPost(int);
private:
	zBOOL  underwaterFX;
	zCOLOR underwaterColor;
	zREAL  underwaterFarZ;
	zREAL  underwaterStartTime;
	zREAL  oldFovX;
	zREAL  oldFovY;
	zCVob* vobUnderwaterPFX;

	//Screen Poly? Für Blenden? Dafür gibts doch die Kamera...
	zCPolygon*           scrPoly;
	zCMesh*              scrPolyMesh;
	int                  scrPolyAlpha;
	zCOLOR               scrPolyColor;
	zTRnd_AlphaBlendFunc scrPolyAlphaFunc;
};

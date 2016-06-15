class zCSkyControler_Mid : public zCSkyControler {
	Z_OBJECT(zCSkyControler_Mid);
public:
	zCSkyControler_Mid()
		: zCSkyControler()
	{
		InitScreenBlend();
	}

	~zCSkyControler_Mid() override
	{
		Release(vobUnderwaterPFX);
		Release(scrPolyMesh);
	}

	virtual void SetUnderwaterFX(int b)
	{
		if (underwaterFX != b) {
			underwaterFX = b;
			if (zCCamera::activeCam) {
				if (underwaterFX) {
					underwaterStartTime = ztimer.totalTimeFloat;
					zCCamera::activeCam->GetFOV(oldFovX, oldFovY);
					zsound->SetGlobalOcclusion(0.8);
				} else {
					zCCamera::activeCam->SetFOV(oldFovX, oldFovY);
					zsound->SetGlobalOcclusion(0.0);
				}
			}
		}
	}

	virtual int GetUnderwaterFX()
	{
		return underwaterFX;
	}

	virtual void RenderSkyPre();
	virtual void RenderSkyPost(int);

	void SetScreenBlendColor(zCOLOR const& color)
	{
		scrPolyColor = color;
	}

	void SetScreenBlendAlpha(int alpha)
	{
		scrPolyAlpha = alpha * zSunMaxScreenBlendScale;
	}

private:
	zBOOL  underwaterFX = false;
	zCOLOR underwaterColor{0xFF003232};
	zREAL  underwaterFarZ = 3500.0;
	zREAL  underwaterStartTime;
	zREAL  oldFovX;
	zREAL  oldFovY;
	zCVob* vobUnderwaterPFX = nullptr;

	//Screen Poly? Für Blenden? Dafür gibts doch die Kamera...
	zCPolygon*           scrPoly;
	zCMesh*              scrPolyMesh;
	int                  scrPolyAlpha;
	zCOLOR               scrPolyColor;
	zTRnd_AlphaBlendFunc scrPolyAlphaFunc;
};

void zCSkyControler_Mid::RenderSkyPre()
{
	if (GetUnderwaterFX()) {
		float time = ztimer.totalTimeFloat - underwaterStartTime;
		float a1 = 1.0 - time * 0.0005;

		if ( a1 < 0.0 )
			a1 = 0.0;

		a2 = 1.0 - zCosApprox(a1 * 1.5707964);

		a3 = 48.0 * a2 + 5.0;
		a4 = a2 + 1.05;

		float fovx = sin((time * 0.001  + Pi)       * a4) *  a3;
		float fovy = sin((time * 0.0006 + (3*Pi/2)) * a4) * (a3 * 0.75);

		zCCamera::activeCam->SetFOV(fovx + oldFovX, fovy + oldFovY);
		zCCamera::activeCam->Activate();
	}
}

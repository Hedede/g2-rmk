struct myThunder {
	myThunder() = default;
	char unk[0x4C];
};

/* Ist in Gothic 2 fürs Rendern von Wetter-Blitzen verantwortlich
 * hier heran kommt man über MEM_SkyController.barrier */
struct oCBarrier {
	void AddTremor(zTRenderContext& ctx) {}
	void RenderThunderList(zTRenderContext& renderContext)
private:
	zCMesh*     skySphereMesh;

	myPoly*      myPolyList;
	myVert*      myVertList;

	int numMyVerts;
	int numMyPolys;

	myThunder*   myThunderList;
	int          numMaxThunders;
	int          numMyThunders;

	int actualIndex;
	int rootBoltIndex;

	int startPointList1[10];
	int numStartPoints1;
	int startPointList2[10];
	int numStartPoints2;
	int startPointList3[10];
	int numStartPoints3;
	int startPointList4[10];
	int numStartPoints4;

	int topestPoint;

	zBOOL bFadeInOut;
	int fadeState;

	zBOOL fadeIn;
	zBOOL fadeOut;
	zBOOL bFlashing;
	float fFlashTime;


	zCSoundFX*                sfx1;
	zTSoundHandle             sfxHandle1;
	zCSoundFX*                sfx2;
	zTSoundHandle             sfxHandle2;
	zCSoundFX*                sfx3;
	zTSoundHandle             sfxHandle3;
	zCSoundFX*                sfx4;
	zTSoundHandle             sfxHandle4;

	zCDecal*               thunderStartDecal;

	zBOOL 		       activeThunder_Sector[4];

	zVEC2*                 originalTexUVList;
};

class oCSkyControler_Barrier : public zCSkyController_Outdoor {
public:
	oCSkyControler_Barrier()
		: zCSkyControler_Outdoor(1)
	{
		barrier = new oCBarrier;
		bFadeInOut = true;
	}

	~oCSkyControler_Barrier()
	{
		DELETE(barrier);
	}

	void RenderSkyPre() override;

private:
	oCBarrier* barrier;
	zBOOL bFadeInOut;
};

//------------------------------------------------------------------------------
void oCSkyControler_Barrier::RenderSkyPre()
{
	zCSkyControler_Outdoor:: RenderSkyPre();

	static bool barrier_initialized = false;
	if (!barrier_initialized)
	{
		barrier_initialized = true;
		barrier->Init();
	}

	auto activeCtrl = zDYNAMIC_CAST<zCSkyControler_Outdoor>(s_activeSkyControler);
	if (!activeCtrl)
		return;

	if (s_skyEffectsEnabled &&
	    activeCtrl->rainFX.outdoorRainFXWeight > 0.5 &&
	    activeCtrl->GetRenderLightning() &&
	    activeCtrl->GetWeatherType())
	{
		zTRenderContext ctx;
		ctx.clipFlags = -1;
		ctx.camera = zCCamera::activeCam;
		ctx.cameraVob= zCCamera::activeCam->connectedVob;
		ctx.cameraWorld = zCCamera::activeCam->connectedVob->homeWorld;
		barrier->Render(ctx,0,0);
	}
	else
	{
		zrenderMan.__skyEffects = 0;
	}
}



void oCBarrier::RenderThunderList(zTRenderContext& renderContext)
{
	for (int i = 0; i < numMaxThunders; ++i)
		RenderThunder(myThunderList[i], renderContext);
}

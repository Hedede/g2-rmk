#include <Logging/Log.h>
#include <Gothic/Game/zWorld.h>

bool oCWorld::HasLevelName( )
{
	if (std::string_view{levelName} != std::string_view{worldFilename}) {
		std::string a(levelName);
		std::string b(worldFilename);
		g2::Log("World", "World names aren't equal: " + a + " | " + b);
	}
	if (levelName)
		return true;
	if (worldFilename)
		return true;
	return false;
}

#include <Gothic/System/zResourceManager.h>
#include <Gothic/System/zCache.h>
#include <Gothic/Graphics/zVertex.h>
#include <Gothic/Graphics/zPolygon.h>
#include <Gothic/Graphics/zRenderer.h>
#include <Gothic/Graphics/zLineCache.h>
#include <Gothic/Graphics/zCamera.h>
#include <Gothic/Graphics/zFFT.h>
#include <Gothic/Menu/zView.h>
#include <Gothic/Cutscene/zCSPlayer.h>
#include <Gothic/Game/zEventManager.h>
#include <Gothic/Game/zVob.h>
struct zCWorldPerFrameCallback {
	void (__thiscall **vtab)(void*,zCWorld*, zCCamera*);
	void operator()(zCWorld* world, zCCamera* cam)
	{
		(*vtab)(this, world, cam);
	}
};


void oCWorld::Render(zCCamera *cam)
{
	if (!cam)
		return;
	if (!cam->connectedVob)
		return;

	if ( cam->connectedVob->homeWorld != this)
		return;

	bool hasLevelName = HasLevelName();

	zCCacheBase::S_PurgeCaches();

	auto& waveAnisEnabled = Value<int>(0x8A8ABC);
	if ( hasLevelName && waveAnisEnabled )
		zCFFT::S_SetFrameCtr(masterFrameCtr);

	cam->PreRenderProcessing();
	cam->UpdateViewport();
	cam->Activate();

	if ( bspTree.bspTreeMode == MODE_INDOOR )
		activeSkyControler = skyControlerIndoor;
	else
		activeSkyControler = skyControlerOutdoor;
	auto& s_activeSkyControler = Value<zCSkyControler*>(0x99AC8C);
	s_activeSkyControler = activeSkyControler;

	if ( renderingFirstTime ) {
		zresMan->cacheInImmedMsec = 1500.0;
		renderingFirstTime = 0;
	}

	if ( hasLevelName ) {
		//zsound->SetListener(zCCamera::activeCam->connectedVob);
		Thiscall<void(oCWorld*)> ProcessZones{0x6207F0};
		ProcessZones(this);
		//zsound->DoSoundUpdate();
	}

	zCPolygon::PrepareRendering();

	bspTree.vobFarClipZ = vobFarClipZScalability * vobFarClipZ;
	bspTree.Render(/*cam*/);
	cam->PostRenderProcessing();
	zrenderer->FlushPolys();

	zCPolygon::S_ResetMorphedVerts();

	if ( hasLevelName ) {
		zlineCache.Flush(cam->targetView);
		//RenderWaynet(cam);
		//ShowZonesDebugInfo();
		//ShowTextureStats();
	}

	cam->Activate();

	MoveVobs();

	if ( hasLevelName ) {
		zCEventManager::DoFrameActivity();
		zresMan->DoFrameActivity();

		if ( csPlayer )
			csPlayer->Process();

		for (auto callback : perFrameCallbackList)
			(*callback)(this, cam);
	}

	++masterFrameCtr;
	zCVertex::ResetVertexTransforms();
	auto& zCTexAniCtrl__masterFrameCtr = Value<unsigned>(0x8D8774);
	++zCTexAniCtrl__masterFrameCtr;
}

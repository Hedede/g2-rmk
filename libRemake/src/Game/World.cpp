#include <Logging/Log.h>
#include <Gothic/Game/zWorld.h>
#include <Gothic/Game/zVob.h>
#include <Gothic/Game/zOptions.h>
zCClassDef* oCWorld::classDef = reinterpret_cast<zCClassDef*>(0xAB37A8);

#include <TypeInfo/oCWorld.h>

static void __thiscall Unarchive_thunk(zCWorld* world, zCArchiver& arc)
{
	g2::Log("World", "Unarchiving world");
	
	Thiscall<void(oCWorld*, zCArchiver&)> call{0x77F860};
	call(static_cast<oCWorld*>(world), arc);

	g2::Log("World", "Unarchiving world done");
}

oCWorld_TypeInfo::oCWorld_TypeInfo()
{
	vt = *reinterpret_cast<zCWorld_vt*>(0x83E174);

	if (zoptions->ReadBool("REMAKE", "use_default_oworld", false))
		return;

	vt.Unarchive = Unarchive_thunk;
}

oCWorld::oCWorld()
{
	Thiscall<void(oCWorld*)> ctor{0x77ED80};
	ctor(this);

	static oCWorld_TypeInfo typeinfo;
	_vtab = &typeinfo.vt;
}

bool oCWorld::HasLevelName( )
{
	if (levelName)
		return true;
	if (worldFilename)
		return true;
	return false;
}

void zCWorld::MoveVobs()
{
	walkList = activeVobList;

	for (auto vob : walkList) {
		if (vob)
			vob->DoFrameActivity();
	}

	walkList.DeleteList();
}

#include <Gothic/System/zResourceManager.h>
#include <Gothic/System/zCache.h>
#include <Gothic/Game/zOptions.h>
#include <Gothic/Graphics/zVertex.h>
#include <Gothic/Graphics/zPolygon.h>
#include <Gothic/Graphics/zRenderer.h>
#include <Gothic/Graphics/zLineCache.h>
#include <Gothic/Graphics/zCamera.h>
#include <Gothic/Graphics/zFFT.h>
#include <Gothic/Audio/zSound.h>
#include <Gothic/Menu/zView.h>
#include <Gothic/Cutscene/zCSPlayer.h>
#include <Gothic/Game/zEventManager.h>
struct zCWorldPerFrameCallback {
	void (__thiscall **vtab)(void*,zCWorld*, zCCamera*);
	void operator()(zCWorld* world, zCCamera* cam)
	{
		(*vtab)(this, world, cam);
	}
};


void oCWorld::Render(zCCamera *cam)
{
	static bool orig_wld_render = zoptions->ReadBool("REMAKE", "orig_wld_render", false);
	if (orig_wld_render) {
		Thiscall<void(oCWorld*, zCCamera*)> call{0x621700};
		return call(this,cam);
	}

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
		zsound->SetListener(zCCamera::activeCam->connectedVob);
		ProcessZones();
		zsound->DoSoundUpdate();
	}

	zCPolygon::PrepareRendering();

	bspTree.vobFarClipZ = vobFarClipZScalability * vobFarClipZ;
	bspTree.Render(cam);
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

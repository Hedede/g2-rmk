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


static int __thiscall LoadWorld_thunk(zCWorld* world, const zSTRING& name, zTWorldLoadMode mode)
{
	return static_cast<oCWorld*>(world)->LoadWorld(std::string_view(name), mode);
}

oCWorld_TypeInfo::oCWorld_TypeInfo()
{
	vt = *reinterpret_cast<zCWorld_vt*>(0x83E174);

	if (zoptions->ReadBool("REMAKE", "use_default_oworld", false))
		return;

	vt.Unarchive = Unarchive_thunk;
	vt.LoadWorld = LoadWorld_thunk;
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

#include <Filesystem.h>
#include <aw/utility/string/case.h>
bool zCWorld::LoadWorld(std::string_view fileName, zTWorldLoadMode mode)
{
	g2::Log("D: WORLD", "Loading WorldFile.. ", fileName);

	zCWorld::s_worldLoadMode = mode;

	DisposeVobs(nullptr);

	zCVob::ResetIDCtr();

	if (aw::in(mode,0,2,3,4))
		DisposeStaticWorld();

	zvertexBufferMan.StartChangeWorld();

	zoptions->ChangeDir(DIR_WORLD);

	g2::Log("D: WORLD", "Opening ZEN archive...");
	auto arc = zarcFactory.CreateArchiverRead(fileName, 0);
	bool ret = arc != nullptr;

	if ( arc ) {
		g2::Log("D: WORLD", "Reading world...");
		arc->ReadObject(this);
		g2::Log("D: WORLD", "Closing archive");
		arc->Close();
		arc->Release();
	}

	zvertexBufferMan.EndChangeWorld();

	zCWorld::s_bFadeOutFarVerts = zoptions->ReadBool("ENGINE", "zFarClipAlphaFade", 1);

	if (compiled && bspTree.bspTreeMode == 0)
		zCWorld::s_bFadeOutFarVerts = 0;

	g2::Log("D: WORLD", " ", fileName);

	if ( ret ) {
		fs::path path(fileName);
		auto filename = path.stem().string();

		levelName = aw::string::toupper(filename);

		bspTree.renderedFirstTime = 1;
	}

	return ret;
}

bool oCWorld::LoadWorld(std::string_view fileName, zTWorldLoadMode mode)
{
	g2::Log("U: (oCWorld::LoadWorld)", fileName);

	auto fname = std::string(fileName);
	aw::string::toupper(fname);

	fs::path fpath(fname);

	if ( mode != zWLD_LOAD_GAME_SAVED_DYN ) {
		worldFilename = fname;
		worldName = fpath.stem().generic_u8string();
	}

	if ( fpath.extension() == ".3DS" )
	{
		auto compo = new zCVobLevelCompo();
		compo->SetVobName("Level-Vob");
		compo->SetVisual(fname);
		AddVob(compo);
		compo->Release();

		return 1;
	}

	zoptions->ChangeDir(DIR_WORLD);
	return zCWorld::LoadWorld(fname, mode);
}

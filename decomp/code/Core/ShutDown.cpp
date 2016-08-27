void zUlfi_ShutDown()
{
	delete zfontman;
}

void oBert_ShutDown() { }
void zBert_ShutDown() { }

void zDieter_ShutDown()
{
	zCRenderLightContainer::S_ReleaseVobLightRefs();
	zCProgMeshProto::CleanupProgMeshProto();
	zCParticleFX::CleanupParticleFX();
	zCDecal::CleanupDecals();
	zCVobLight::CleanupVobLight();
	zCLensFlareFX::ReleaseLensFlareScript();
	zCVob::CleanupVobSystem();
	zCVisual::CleanupVisualSystem();
	zCMapDetailTexture::S_Cleanup();
	zCRenderManager::CleanupRenderManager();
	zresMan->PurgeCaches(0);
	Delete( zsndMan );
	Delete( zrenderer );
	Delete( zsound );
}

void zDieter_ShutDown_Fast()
{
	zCRenderManager::CleanupRenderManager();
}

void zCarsten_ShutDown()
{
	zmusic = 0;
	zCMenu::Shutdown();
	Delete( zinput );
	zCAICamera::GlobalCleanUp();
}

void zCarsten_ShutDown_Fast()
{
	Delete( zinput );
}

void oCarsten_ShutDown()
{
	Delete( oCVisualFX::fxParser );
}


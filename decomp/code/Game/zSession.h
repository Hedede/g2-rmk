struct zCSession : zCInputCallback {
	zCSession() = default;
	virtual ~zCSession()
	{
		Done();
	}

	virtual void Init();

	virtual void Done();
	virtual void Render();
	virtual void RenderBlit();
	virtual void CamInit(zCVob *,zCCamera *);
	virtual void CamInit();

	virtual void SetCamera(zCCamera *);
	virtual void GetCamera() const;
	virtual void GetCameraAI() const;
	virtual void GetCameraVob() const;
	virtual void GetViewport() const;

	virtual void SaveWorld(zSTRING const&,zCWorld::zTWorldSaveMode,int,int);
	virtual void LoadWorld(zSTRING const&,zCWorld::zTWorldLoadMode) {};

	virtual void GetCutsceneManager();

	virtual void SetTime(int,int,int) {}
	virtual void GetTime(int&,int&,int&) const {}

	virtual void GetWorld() const;
	virtual void SetWorld(zCWorld*);

	virtual void DesktopInit();
	virtual void CutsceneSystemInit();

	void CamInit(zCVob* camVob, zCCamera* camera);
	void CutsceneSystemInit();

private:
	zCCSManager*  csMan    = 0;
	zCWorld*      world    = 0;
	zCCamera*     camera   = 0;
	zCAICamera*   aiCam    = 0;
	zCVob*        camVob   = 0;
	zCView*       viewport = 0;
};

//-----------------------------------------------------------------------------
void zCSession::Init()
{
	DesktopInit();
	zcon.SetWorld(world);
	zcon.SetParser(&zparser);
	sysEvent();
	CutsceneSystemInit();
	sysEvent();
}

void zCSession::CamInit(zCVob *theCamVob, zCCamera *theCamera)
{
	this->camera = theCamera;
	camera->SetRenderTarget(viewport);

	this->camVob = theCamVob;
	world->AddVob(camVob);

	camVob->SetCollDetStat(0);
	camVob->SetCollDetDyn(0);
	camVob->SetSleeping(0);
	camVob->SetPhysicsEnabled(0);

	camera->connectedVob = camVob;

	zsound->SetListener(camVob);
}

void zCSession::CamInit()
{
	Delete(camera);

	camera = new zCCamera();
	camera->SetRenderTarget(viewport);

	Release(aiCam);

	camVob->SetAI(0);
	Release(camVob);

	aiCam = zCAICamera::Create();

	camVob = new zCVob();
	camVob->SetObjectName("ZCAICAMERA");
	if ( !camVob->homeWorld )
		world->AddVob(camVob);
	aiCam->SetVob(camVob);

	camVob->SetCollDetStat(0);
	camVob->SetCollDetDyn(0);
	camVob->SetAI(aiCam);
	camVob->SetSleeping(0);
	camVob->SetPhysicsEnabled(0);

	camera->connectedVob = camVob;
	zsound->SetListener(camVob);
}

void zCSession::DesktopInit()
{
	viewport = new zCView(0, 0, 0x1FFF, 0x1FFF, 1);
	screen->InsertItem(viewport, 0);
	viewport->SetFont(0);
}

void zCSession::CutsceneSystemInit()
{
	if ( !csMan )
		csMan = zfactory->CreateCSManager();
	csMan->LibCheckLoaded(3);
}

void zCSession::Done()
{
	Release(world);
	Release(aiCam);
	camVob->SetAI(0);
	Release(camVob);
	Release(csMan);
	Delete(camera);
	Delete(viewport);
}

zCCamera* zCSession::GetCamera() const
{
	return this->camera;
}

zCAICamera* zCSession::GetCameraAI() const
{
	return this->aiCam;
}

zCVob* zCSession::GetCameraVob() const
{
	return this->camVob;
}

void zCSession::SetCamera(zCCamera *camera)
{
	this->camera = camera;
}

zCWorld* zCSession::GetWorld() const
{
	return world;
}

void zCSession::SetWorld(zCWorld* world)
{
	this->world = world;
	world->SetOwnerSession(this);
}

zCCSManager* zCSession::GetCutsceneManager() const
{
	return csMan;
}

zCView* zCSession::GetViewport() const
{
  return this->viewport;
}

void zCSession::RenderBlit()
{
	zrenderer->Vid_Blit(1, 0, 0);
}

void zCSession::Render()
{
	world->Render(camera);
	zrenderer->Vid_BeginLfbAccess();
	screen->DrawItems();
	zrenderer->Vid_EndLfbAccess();
	zrenderer->Vid_Unlock();
}


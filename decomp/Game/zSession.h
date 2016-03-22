class zCSession : public zCInputCallback {
public:
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

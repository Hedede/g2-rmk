class zCSession {
public:
	virtual ~zCSession();

	virtual void Init();
	virtual void Done();
	virtual void Render();
	virtual void RenderBlit();
	virtual void CamInit(zCVob *,zCCamera *);
	virtual void CamInit();
	virtual void SetCamera(zCCamera *);
	virtual void GetCamera();
	virtual void GetCameraAI();
	virtual void GetCameraVob();
	virtual void GetViewport();
	virtual void GetWorld();
	virtual void SaveWorld(zSTRING const &,zCWorld::zTWorldSaveMode,int,int);

	virtual void LoadWorld(zSTRING const &,zCWorld::zTWorldLoadMode);
	virtual void GetCutsceneManager();
	virtual void SetTime(int,int,int);
	virtual void GetTime(int &,int &,int &);
	virtual void SetWorld(zCWorld	*);

	virtual void DesktopInit();
	virtual void CutsceneSystemInit();

	void CamInit(zCVob* camVob, zCCamera* camera);
private:
	zCCSManager*  csMan;
	zCWorld*      world;
	zCCamera*     camera;
	zCAICamera*   aiCam;
	zCVob*        camVob;
	zCView*       viewport;
};

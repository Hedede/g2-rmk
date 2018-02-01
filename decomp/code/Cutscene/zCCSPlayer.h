class zCCSPlayer : public zCObject, public zCCSDebuger {
	Z_OBJECT(zCCSPlayer);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCSPlayer();
	virtual void CreateNewOutputUnit(int,zCVob *);
	virtual void StartOutputUnit(int,zCVob *);
	virtual void StartCutscene(zCCutscene *,zCVob *,zSTRING);
	virtual void StartCutscene(zSTRING &,zCVob *);
	virtual void ForgetCutscene(zCCutscene *);
	virtual void ResetCutscenePlayer();
	virtual void StopAllCutscenes(zCVob *);
	virtual void StopAllOutputUnits(zCVob *);
	virtual void InsertCutscene(zCCSCutsceneContext *);
	virtual void RemoveCutscene(zCCSCutsceneContext *);
	virtual void OnCSTrigger(zSTRING &,zCVob *);
	virtual void OnCSUntrigger(zSTRING &,zCVob *);
	virtual void SetOwnerWorld(zCWorld *);
	virtual void SetOwnerManager(zCCSManager *);
	virtual void Process()
	{
		if (UpdateCutscenesNow()) {
			int day, hour, min;
			wld->GetOwnerSession()->GetTime(day, hour, min);
			auto cam = wld->GetOwnerSession()->GetCameraVob();
			auto pos = cam->GetPositionWorld();
			ProcessList( pos, day, hour, min );
		}
	}
	virtual void ProcessCutscene(zCCSCutsceneContext *,zVEC3 const &);
	virtual void ProcessList(zVEC3 const &,int,int,int);
	virtual void CSDB_BeginSyncBlock(zCCSSyncBlock *);
	virtual void CSDB_BeginAtomBlock(zCCSBlockBase *);
	virtual void CSDB_EndAtomBlock(zCCSBlockBase *);
	virtual void CSDB_Error(zCCSBlockBase *,zSTRING &);
	virtual void CSDB_Warning(zCCSBlockBase *,zSTRING &);
	virtual void CSDB_StartPlaying(zCCutscene *);
	virtual void CSDB_Stop(zCCutscene *);

	void Resume()
	{
		// GetNumInList() and Get() make this O(n²) instead of O(n)!
		// There are also examples of O(n³) algorithms
		// on linked lists in the game code...
		for (int i = 0; i < playList.GetNumInList(); ++i) {
			if ( playList.Get(i)->IsFinished() )
				continue;

			// also for some reason compiler inserted
			// (nullptr)->ResumeActBlock(0)
			playList.Get(i)->ResumeActBlock(0)
		}
	}

	void ToggleDebugInfo()
	{
		__debugShow = !__debugShow;
		if ( !__debugView )
			// TODO: ,2);  — don't forget to add def arg to zCView ctor
			__debugView = new zCView(0,500,8192,8000); // 907
		if ( __debugShow ) {
			screen->InsertItem(__debugView, 0);
			DebugResetInfoscreen();
		} else {
			screen->RemoveItem(__debugView);
		}
	}

protected:
	int UpdateCutscenesNow()
	{
		if ( playList.GetNumInList() > 0 ) {
			if (__timer < 250.0) {
				__timer += ztimer.frameTimeFloat;
				return 0;
			} else {
				__timer = 0.0;
				return 1;
			}
		} else {
			DebugResetInfoscreen();
		}
		return 0;
	}
private:
	int lastProcessDay;
	int lastProcessHour;
	int unk0[2];
	zBOOL __debugShow;
	zCView *__debugView;
	float __timer;
	CsContextList playList;
	zCCSManager *csMan;
	int wtd;
};

void zCCSPlayer::Archive(zCArchiver& arc)
{
	zINFO(5,"B: CS: Archiving cutsceneplayer ...");
	if (arc.InSaveGame()) {
		arc.WriteInt("lastProcessDay",  lastProcessDay);
		arc.WriteInt("lastProcessHour", lastProcessHour);

		int count = playList.GetNumInList();
		arc.WriteInt("playListCount", count);

		for (int i = 0; i < count; ++i) {
			auto context = playList.Get(i);
			arc.WriteObject("playContext" + i, context);
		}
	}
	zINFO(5, "B: CS: Cutsceneplayer archived.");
}

void zCCSPlayer::Unarchive(zCArchiver& arc);
{
	zINFO(5,"B: CS: Unarchiving cutsceneplayer ...");
	for (int i = 0, num = playList.GetNumInList(); i < num; ++i) {
		if (playList.Get(i)) {
			playList.Get(i).Stop();
			playList.Get(i).Release();
		}
	}

	playList.DeleteList();

	if (arc.InSaveGame()) {
		arc.ReadInt("lastProcessDay",  lastProcessDay);
		arc.ReadInt("lastProcessHour", lastProcessHour);

		int count = 0;
		arc.ReadInt("playListCount", count);
		for (int i = 0; i < count; ++i) {
			auto obj = arc.ReadObject("playContext"_s + i, 0);
			auto cscontext = dynamic_cast<zCCSCutsceneContext>(obj);

			if (cscontext->IsCutscene())
				csMan->InitiateCSwithContext(context);
			else
				csMan->InitiateOUwithContext(context);

			InsertCutscene(context);
		}
	}
	zINFO(5, "B: CS: Cutsceneplayer unarchived.");
}

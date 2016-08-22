class oCAIHuman : public oCAniCtrl_Human {
	Z_OBJECT(oCAIHuman);
public:
	void Archive(zCArchiver& arc) override
	{
		oCAniCtrl_Human::Archive(arc);
		if (arc.InSaveGame()) {
		}
	}

	void Unarchive(zCArchiver& arc) override
	{
		oCAniCtrl_Human::Unarchive(arc);
		if (arc.InSaveGame()) {
		}
	}

	virtual ~oCAIHuman();
	virtual void DoAI(zCVob *,int &);
	virtual bool CanThisCollideWith(zCVob* vob)
	{
		return ignoreVobList.Search(vob) < 0;
	}
	virtual void Init(oCNpc* npc)
	{
		if (npc) {
			oCAniCtrl_Human::Init(npc);
			zCAIPlayer::ResetAutoRoll();
		}
	}
	virtual void AddIgnoreCD(zCVob* vob)
	{
		ignoreVobList.Insert(vob);
	}
	virtual void SubIgnoreCD(zCVob* vob)
	{
		ignoreVobList.Remove(vob);
	}
	virtual void DoHackSpecials();

	// TODO: take from gothic1 (it is commented out in g2)
	void SetCamMode(zSTRING const& activateCamMode, int checkSpecial);
	void ChangeCamMode(zSTRING const& activateCamMode)
	{
		SetCamMode(activateCamMode, 1);
	}
	void CamMessages();
	int MoveCamera() { return 0; }
	void CheckFightCamera();

	void SetShowAI(bool b)
	{
		flags.showai = b;
	}
	int GetShowAI() const
	{
		return flags.showai;
	}

	int Toggled(int key)
	{
		return zinput->GetToggled(key);
	}
	float Pressed(int key)
	{
		return zinput->GetState(key);
	}

protected:
	static void ResetAssesUseMobCounter()
	{
		assessUseMob_counter = ASSESSUSEMOB_TIME; // 3000.0
	}
	static void oCAIHuman::ResetObserveSuspectCounter()
	{
		observeSuspect_Counter = OBSERVESUSPECT_TIME; // 3000.0
	}

	void PC_Turn(float dir, int playAnis)
	{
		oCAniCtrl_Human::Turn(dir, playAnis);
	}

	void DoMonsterSpecial() {}

private:
	//Da oCAIHuman (eine Unterklasse) nicht mehr viel zusätzlich enthält, hier einfach mal drangeklatscht:
	//Ich habe sowieso nicht den Eindruck, dass diese Eigenschaften sehr nüztlich sind.
	zCList<zCVob> ignoreVobList;

	zCAICamera* aiCam;

	struct {
		zBOOL               forcejump               :1;
		zBOOL               lookedAround            :1;
		zBOOL               sprintActive            :1;
		zBOOL               crawlActive             :1;
		zBOOL               showai                  :1;
		zBOOL               startObserveIntruder    :1;
		zBOOL               dontKnowAniPlayed       :1;
		zBOOL               spellReleased           :1;
		zBOOL               spellCastedLastFrame    :1;
		zBOOL               eyeBlinkActivated       :1;
		zBOOL               thirdPersonFallback     :1;
	} flags;

	zREAL oCAIHuman_createFlyDamage;
};

void oCAIHuman::~oCAIHuman()
{
	if ( --s_overlayRefCtr <= 0 ) {
		if ( s_sprintOverlay ) {
			s_sprintOverlay->Release();
			s_sprintOverlay = 0;
		}
		if ( s_crawlOverlay ) {
			s_crawlOverlay->Release();
			s_crawlOverlay = 0;
		}
	}

	SetSprintMode(0);
	SetCrawlMode(0);

	aiCam = 0;

	ignoreVobList.DeleteList();
}

void oCAIHuman::CheckFightCamera()
{
	if ( enemyList.GetNum() <= 0 && !npc->GetWeaponMode() ) {
		auto cam = zCAICamera::GetCurrent();
		if ( cam->IsModeActive(oCAIHuman::Cam_Fight) ) {
			if ( walkmode )
				SetCamMode(oCAIHuman::Cam_Normal, 0);
			else
				SetCamMode(oCAIHuman::Cam_Run, 0);
		}
	}
}

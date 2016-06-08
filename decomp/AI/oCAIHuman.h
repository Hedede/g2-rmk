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
	virtual void CanThisCollideWith(zCVob*);
	virtual void Init(oCNpc *);
	virtual void AddIgnoreCD(zCVob *);
	virtual void SubIgnoreCD(zCVob *);
	virtual void DoHackSpecials();

	void CamMessages();

private:
	//Da oCAIHuman (eine Unterklasse) nicht mehr viel zusätzlich enthält, hier einfach mal drangeklatscht:
	//Ich habe sowieso nicht den Eindruck, dass diese Eigenschaften sehr nüztlich sind.
	zCList<zCVob> ignoreVobList;

	zCAICamera* oCAIHuman_aiCam;

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

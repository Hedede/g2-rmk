class zCCSCutsceneContext : public zCCSCutscene {
	Z_OBJECT(zCCSCutsceneContext);
public:
	virtual void Archive(zCArchiver&);
	virtual void Unarchive(zCArchiver&);
	virtual ~zCCSCutsceneContext();
	virtual void AddMainRole(zCVob *);
	virtual void StartPlaying();
	virtual void Stop();
	virtual void Interrupt();
	virtual void IsCutscene();
	virtual void IsFinished();
	virtual void PrintDebugInfo(zSTRING const &);
	virtual void GetStage();
	virtual void CatchPlayer(zCVob *);
	virtual void PrepareBlock();
	virtual void SetPoolItem(zCCSPoolItem *);
	virtual void SetStage(zCVob *);
	virtual void Play(zVEC3);
	virtual void ForcedInterrupt();
	virtual void CheckRoleReactions();
	virtual void IsPlaying();
	virtual void IsOutputUnit();

	void HandleUntrigger() {}
};

void zCCSCutsceneContext::Archive(zCArchiver& arc)
{
	zINFO(6,"B: CS: Archiving cutscene ...");
	zINFO(6,"B: CS: -- Cutscenename: " + GetCSName());

	zCCutscene::Archive(arc);

	if (arc.InSaveGame()) {
		arc.WriteBool("isCutscene", isCutscene);
		if ( isCutscene ) {
			arc.WriteBool("allowedToPlay", allowedToPlay);

			zINFO("B: CS: -- Is full featured cutscene");
		} else {
			arc.WriteInt(a3, "reference", reference);

			zINFO(6,"B: CS: -- Is Outputunit cutscene");
		}

		arc.WriteInt("actualBlock", GetActualBlockNum());

		arc.WriteObject("alt_stage", alt_stage);
		arc.WriteObject("stage", stage);

		arc.WriteBool("wasTriggered", wasTriggered);
	}

	zINFO(6,"B: CS: -> Cutscene archived");
}

void zCCSCutsceneContext::Unarchive(zCArchiver& arc)
{
	zINFO(6, "B: CS: Unarchiving cutscene ... ");

	zCCutscene::Unarchive(arc);

	zINFO(6,"B: CS: -- Cutscenename: " + GetCSName());

	if (arc.InSaveGame(arc)) {
		arc.ReadBool("isCutscene", isCutscene);
		if (isCutscene) {
			reference = -1;
			arc.ReadBool("allowedToPlay", allowedToPlay);

			zINFO(6,"B: CS: -- Is full featured cutscene");
		} else {
			arc.ReadInt("reference", reference);

			zINFO(6,"B: CS: -- Is Outputunit cutscene");
		}

		int block = arc.ReadInt("actualBlock");
		ResumeAtBlock(block);

		if ( alt_stage ) {
			alt_stage->Release();
			alt_stage = 0;
		}

		if ( stage ) {
			stage->Release();
			stage = 0;
		}

		auto obj  = arc.ReadObject("alt_stage", 0);
		alt_stage = dynamic_cast<zCVob>(obj);

		obj  = arc.ReadObject("alt_stage", 0);
		stage = dynamic_cast<zCVob>(obj);

		arc.ReadBool("wasTriggered", wasTriggered);

		unk4 = 0xC0000000; // float
	}

	zINFO(6, "CS: -> Cutscene unarchived.");
}

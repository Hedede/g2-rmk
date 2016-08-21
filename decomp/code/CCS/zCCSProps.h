struct zCCSProps : zCObject {
	z_OBEJCT(zCCSProps);
public:
	enum zTCSRunBehaviour {
		RUN_ALWAYS = 0x0,
		RUN_TIMES = 0x1,
		RUN_PERHOUR = 0x2,
		RUN_PERDAY = 0x3,
	};

	virtual void Archive(zCArchiver &);
	virtual void Unarchive(zCArchiver&);
	virtual ~zCCSProps();
	virtual void AcceptRole(zCCSRole* role,zCVob* vob)
	{
		return CheckRole(role, vob);
	}

	virtual void CheckRole(zCCSRole *,zCVob *)
	{
		return 1;
	}
	virtual int GetCheckRoleResult()
	{
		return 2;
	}
	
	virtual bool CheckDistance(float dist) const
	{
		return _ignoreDistance || distance < 0.0 || dist < distance;
	}

	virtual bool CheckRange(float rng) const
	{
		return range < 0.0 || rng < range;
	}

	virtual void PrintDebugInfo();

	zSTRING GetScriptFuncOnStop() const
	{
		return scriptFuncOnStop;
	}

	zSTRING GetName() const
	{
		return name;
	}
private:
	zSTRING name;
	zBOOL csLoop          = false;
	zBOOL globalCutscene  = false;
	zBOOL hasToBeTriggerd = false;
	// In binary distance was before hasToBeTriggerd
	float distance = 500.0;
	float range = 2000.0;
	int _ignoreDistance = 0;
	int numLockedBlocks = 0;
	zSTRING stageName;
	zSTRING scriptFuncOnStop;
	int runBehviour = RUN_ALWAYS;
	int runBehaviourValue = 0;
};

void zCCSProps::Archive(zCArchiver& archive)
{
	if ( archive.InSaveGame(archive) )
		archive.WriteString("name", name);

	archive.WriteBool("globalCutscene", globalCutscene);
	archive.WriteBool("csLoop", csLoop);
	archive.WriteBool("hasToBeTriggerd", hasToBeTriggerd);
	archive.WriteFloat("distance", distance);
	archive.WriteFloat("range", range);
	archive.WriteInt("numLockedBlocks", numLockedBlocks);
	archive.WriteEnum("runBehaviour",
	                  "RUN_ALWAYS;RUN_TIMES;RUN_PERHOUR;RUN_PERDAY",
	                  runBehviour);
	archive.WriteInt("runBehaviourValue", runBehaviourValue);
	archive.WriteString("StageName", stageName);
	archive.WriteString("scriptFuncOnStop", scriptFuncOnStop);
}

void zCCSProps::Unarchive(zCArchiver& archive)
{
	if (archive.InSaveGame(archive))
		archive.ReadString("name", name);

	archive.ReadBool("globalCutscene", globalCutscene);
	archive.ReadBool("csLoop", csLoop);
	archive.ReadBool("hasToBeTriggerd", hasToBeTriggerd);

	archive.ReadFloat("distance", distance);
	archive.ReadFloat("range", range);

	archive.ReadInt("numLockedBlocks", numLockedBlocks);

	archive.ReadEnum("runBehaviour", runBehviour);
	archive.ReadInt("runBehaviourValue", runBehaviourValue);

	archive.ReadString("StageName", stageName);
	archive.ReadString("scriptFuncOnStop", scriptFuncOnStop);
}

void zCCSProps::PrintDebugInfo()
{
	if (globalCutscene)
		zINFO(1, "B: CS: - is a global cutscene.");
	else
		zINFO(5, "B: CS: - is an ambient cutscene.");

	if (csLoop)
		zINFO(1, "B: CS: - loops.");
	else
		zINFO(5, "B: CS: - doesn't loop.");

	if ( hasToBeTriggerd )
		zINFO(1, "B: CS: - has to be triggered.");

	zINFO(1, "B: CS: - distance: " + zSTRING(distance));
	zINFO(1, "B: CS: - range: " + zSTRING(range));

	zSTRING behStr;
	switch ( runBehviour ) {
	case RUN_ALWAYS:
		behStr = "RUN_ALWAYS";
		break;
	case RUN_TIMES:
		behStr = zSTRING(runBehaviourValue) + " times";
		break;
	case RUN_PERHOUR:
		behStr = zSTRING(runBehaviourValue) + " times per hour";
		break;
	case RUN_PERDAY:
		behStr = zSTRING(runBehaviourValue) + " times per day";
		break;
	default:
		behStr = "unknown";
		break;
	}
	zINFO(1, "B: CS: - runBehaviour: " behStr);
}

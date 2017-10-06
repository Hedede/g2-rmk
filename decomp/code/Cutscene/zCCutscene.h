class zCCutscene : public zCCSBlock {
	Z_OBJECT(zCCutscene);
public:
	virtual void Archive(zCArchiver& arc)
	{
		zCCSBlock::Archive(arc);

		arc.WriteObject(properties);

		int numRoles = roles.GetNumInList();
		arc.WriteInt("numOfRoles", numRoles);

		for (int i = 0; i < numRoles; ++i )
			arc.WriteObject(roles[i]);

		int numVobs = roleVobs.GetNumInList();
		arc.WriteInt("numOfRoleVobs", numVobs);

		for (int i = 0; i < numVobs; ++i )
			arc.WriteObject(roleVobs[i]);

		arc.WriteObject("mainRoleVob", mainRoleVob);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCCSBlock::Unarchive(&this->vtbl, arc);

		ClearCutscene();

		Release(properties);

		auto obj = arc.ReadObject(0);
		properties = dynamic_cast<zCCSProps>(obj);

		if ( properties )
			SetObjectName(properties->GetName());

		int numRoles = arc.ReadInt("numOfRoles");
		for (int i = 0; i < numRoles; ++i) {
			obj = arc.ReadObject(0);
			auto role = dynamic_cast<zCCSRole>(obj);
			roles.Insert(role);
		}

		int numVobs = arc.ReadInt("numOfRoleVobs");
		for (int i = 0; i < numVobs; ++i) {
			obj = arc.ReadObject(0);
			auto vob = dynamic_cast<zCVob>(obj);
			roleVobs.Insert(vob);
			eventManagers.Insert(nullptr);
		}

		Release(mainRoleVob);

		obj = arc.ReadObject("mainRoleVob", 0);
		mainRoleVob = dynamic_cast<zCVob*>(obj);
	}

	zCCSProps* GetProperties() { return properties; }
	int GetActualBlockNum() { return blockNum; }
	void ResumeAtBlock(int nr) { blockNum = nr; }

	virtual ~zCCutscene();
	virtual void NewBlock(int,int);
	virtual void AddMainRole(zCVob *);
	virtual void AddRoleOU(zCVob	*);
	virtual void RemoveRole(zCVob *);
	virtual void SetOwnerPlayer(zCCSPlayer *);
	virtual void Reset();
	virtual void ClearCutscene();
	virtual void StartPlaying();
	virtual void Stop();
	virtual void StopRole(zCVob *);
	virtual void Interrupt();
	virtual void ResumeActBlock();
	virtual void PlaySub(float,float);
	virtual void NewCS(zCCSManager *);
	virtual zBOOL IsCutscene() { return true; }
	virtual zBOOL IsFinished()
	{
		return GetNumOfChilds() == GetActualBlockNum();
	}

	virtual void PrintDebugInfo(zSTRING const&) {}
	virtual void CheckRoles(int &);
	virtual void CSCheck(int &);
	virtual zCVob* GetStage() { return nullptr; }
	virtual void LoadStage();
	virtual void CatchPlayer(zCVob*);
	virtual void PrepareBlock();

private:
	int unkro;
	zCArrat<zCCSRole*> roles;
	zCArray<zCEventManager*> eventManagers;
	zCArray<zCVob*> roleVobs;
	zCVob *mainRoleVob;
	zCCSProps *properties;
	int notarray1;
	int blockNum;
	int notarray3;
};

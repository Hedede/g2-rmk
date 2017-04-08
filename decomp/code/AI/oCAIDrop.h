class oCAIDrop : public oCAISound {
	Z_OBJECT();
public:
	oCAIDrop() = default;
	~oCAIDrop()
	{
		ClearIgnoreCDVob();
		Release(inVob);
		Release(shootVob);
	}

	void Archive(zCArchiver& arc) override
	{
		if (arc.InProperties()) {
			// commented out?
		}
	}
	void Unarchive(zCArchiver &) override
	{
		if (arc.InProperties()) {
			// commented out?
		}
	}

	virtual void DoAI(zCVob *,int &);

	bool CanThisCollideWith(zCVob* vob) override
	{
		// dont remember Search usage
		return !ignoreCD.Search(vob);
	}

	void ReportCollisionToAI(zCCollisionReport const& collReport) override
	{
		oCAISound::StartCollisionSound(inVob, collReport);
		if ( inVob )
			oCNpc::AssessQuietSound_S(inVob, inVob->GetPositionWorld());
	}

	bool HasAIDetectedCollision() override
	{
		return false;
	}


	void AddIgnoreCDVob(zCVob* vob)
	{
		if (vob) {
			ignoreCD.Insert(vob);
			++vob->refCtr;
		}
	}

	void ClearIgnoreCDVob()
	{
		for (auto vob : ignoreCD)
			vob->Release();

		ignoreCD.DeleteList();
	}


private:
	zCList<zCVob> ignoreCD;
	oCVob* inVob = 0;
	oCVob* shootVob = 0;
	int unk2 = 0;
	int unk3 = 0;
	float ctr = 0; // some peg
};

void oCAIDrop::SetupAIVob(zCVob *inVob, zCVob *shootVob)
{
	ClearIgnoreCDVob();

	Release(shootVob);
	Release(inVob);

	this->inVob = inVob;
	this->shootVob = shootVob;

	AddRef(inVob);
	AddRef(shootVob);

	this->drop1 = 0;
	LODWORD(this->__ctr) = 0;

	auto pos = shootVob.GetTrafoModelNodeToWorld("ZS_RIGHTHAND").GetTranslation();

	inVob->SetCollDetStat(0);
	inVob->SetCollDetDyn(0);
	if ( !inVob->homeWorld )
		ogame->GetGameWorld()->EnableVob(inVob, 0);

	inVob->ResetRotationsWorld();
	SetStartPosition(pos);

	auto vel = shootVob->GetAtVectorWorld() * 50.0;
	inVob->GetRigidBody()->SetVelocity(vel);
	inVob->GetRigidBody()->flags.1 = 1;
	inVob.SetSleeping(0);
	inVob.SetSleeping(1);

	AddIgnoreCDVob(shootVob);

	this->drop2 = 0;

	if ( inVob->type == VOB_TYPE_ITEM )
		inVob.ClearFlag(0x800000);

	inVob.SetCollDetStat(0);
	inVob.SetCollDetDyn(0);
}

void oCAIDrop::SetStartPosition(zVEC3& pos)
{
	inVob.SetCollDetStat(0);
	inVob.SetCollDetDyn(0);
	inVob.SetPositionWorld(pos);
	inVob.ResetRotationsWorld();

	if ( inVob ) {
		if (auto vob = zDYNAMIC_CAST<oCVob>(inVob)) {
			if ( vob->SearchFreePosition(pos, shootVob) ) {
				vob->SetPositionWorld(pos);
			} else {
				zWARNING("U:DROP:Not enough space for object.");
			}
		}
	}

	inVob.SetCollDetStat(1);
	inVob.SetCollDetDyn(1);
}

void oCAIDrop::SetVelocity(float f, float angle)
{
	zVEC3 axis {1, 0, 0};
	zMAT4 rot = Alg_Rotation3DN(axis, -angle);

	auto mat = shootVob->trafoObjToWorld * rot;
	axis = mat.GetAtVector() * f;
	inVob->GetRigidBody()->SetVelocity(axis);
}

void oCAIDrop::DoAI(zCVob *inVob, int *vobDeleted)
{
	ctr += 1.0;
	if ( !(inVob->flags1.physicsEnabled) )
	{
		if ( ctr <= 10.0 ) {
			inVob.SetPhysicsEnabled(1);
		} else {
			inVob.SetCollDetStat(1);
			inVob.SetCollDetDyn(1);
			inVob.SetSleeping(1);
			inVob.SetAI(0);
		}
	}
}

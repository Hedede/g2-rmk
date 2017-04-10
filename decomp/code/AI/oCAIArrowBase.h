class oCAIArrowBase : public oCAISound {
	Z_OBJECT(oCAIArrowBase);
public:
	oCAIArrowBase() = default;
	~oCAIArrowBase()
	{
		Release(vob);
		if (trailVob)
			trailVob->homeWorld->RemoveVob(trailVob);
		Release(trailVob);
		Release(trail);
		ignoreCD.DeleteList();
	}
	void Archive(zCArchiver& arc) override
	{
		if ( !arc.InProperties() && arc.InSaveGame() ) {
			zCVob* vob = this->vob;
			if ( oCNpc::dontArchiveThisNpc == vob )
				vob = nullptr;
			arc.WriteObject("vob", vob);
			arc.WriteBool("collision", collision);
			arc.WriteFloat("timeLeft", timeLeft);
			arc.WriteBool("startDustFX", startDustFX);
		}
	}
	void Unarchive(zCArchiver& arc) override
	{
		if ( !arc.InProperties() && arc.InSaveGame() ) {
			if ( vob ) {
				vob->Release();
				vob = 0;
			}

			vob = arc.ReadObject("vob", 0);
			arc.ReadBool("collision", collision);
			arc.ReadFloat("timeLeft", timeLeft);
			arc.ReadBool("startDustFX", startDustFX);
			if ( unk1 ) {
				unk1->Release();
				unk1 = 0;
			}
		}
	}
	void DoAI(zCVob* vob,int& vobRemoved) override;
	void ReportCollisionToAI(zCCollisionReport const& colReport) override;
	bool HasAIDetectedCollision() override { return false; }
	void AICollisionResponseSelfDetected(zVEC3 const &,int &) override {}
	bool GetIsProjectile() override { return true; }


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
	zBOOL collision = 0;
	float timeLeft  = -1.0;
	zCVob *vob      = nullptr;
	zBOOL startDustFX = false;
	zCVob *trailVob    = nullptr;
	zCPolyStrip *trail = nullptr;
	int unknown = 0;
	float trailTime;
	int __ignoreArmor = true;
};

void oCAIArrowBase::CreateTrail(zCVob* parent)
{
	trail = new zCPolyStrip;
	trail->__visible = 1;
	trail->fadeSpeed = 0.3;
	trail->mat->SetTexture("ARROWSTRIP.TGA");
	trail->mat->alphaFunc = 2;
	trail->mat->aniCtrl.aniFPS = 0.01;
	trail->mat->aniCtrl.isOneShot = 1;
	trail->mat->color.alpha = 255;
	trail->AllocateResources(64);
	trail->__manual = 1;
	trailTime = TRAIL_STRIP_SEGMENT_TIME;

	trailVob = new zCVob;
	trailVob.flags3.0x10 = 1;
	trailVob.SetVisual(trail);

	if (auto pm = zDYNAMIC_CAST<zCProgMeshProto>(trailVob->visual))
	{
		auto mat = pm->subMeshes->material;
		mat->alphaFunc = 3;
		mat->color.alpha = 255;
	}

	if ( parent->homeWorld ) {
		auto bb = parent->GetBBox3DLocal();
		trailVob->BeginMovement();
		auto trafo = parent->trafoObjToWorld;
		auto pos   = trafo.GetTranslation();
		trailVob->SetTrafoObjToWorld(trafo);
		trailVob->SetPositionWorld(pos);
		trailVob->MoveLocal(0, 0, pos.z);
		trailVob.EndMovement(1);
		parent->homeWorld->AddVobAsChild(trailVob, parent);
	}
}

void oCAIArrowBase::ReportCollisionToAI(zCCollisionReport const& colReport)
{
	if ( !startDustFX )
		return;

	vob->SetSleeping(0);
	if (timeLeft == -1.0)
		timeLeft = 1.0;
	auto col2 = colReport.col2;
	if ( !col2 )
		return;

	if (col2->GetCollObjClass() == &zCCollObjectLevelPolys::s_oCollObjClass ) {
		startDustFX = 1;
		if ( !collision ) {
			// FIND() is pseudocode
			if ( FIND(col2.polys, IT->material->matGroup == MAT_WOOD) ) {
				vob->SetCollDetStat(0);
				vob->SetCollDetDyn(0);
				vob->SetPhysicsEnabled(0);
				auto right = vob->trafoObjToWorld->GetRightVector();
				vob->Move(-right * 15.0);
			}
		}

		vob->GetRigidBody()->flags.1 = 1;
		auto vel = vob->GetRigidBody()->vel * 0.8;
		vob->GetRigidBody()->SetVelocity(vel);

		collision = 1;
	} else {
		// inlined?
		if ( col2 ) {
			if (auto npc = zDYNAMIC_CAST<oCNpc>(col2->parent)) {
				auto armor = npc->GetEquippedArmor();
				if (!armor || armor->GetEquippedArmor() != SND_MAT_METAL || __ignoreArmor) {
					vob->SetPhysicsEnabled(0);
					if (timeLeft > 0.1)
						timeLeft = 0.05;
				}
			} else if (auto pm = zDYNAMIC_CAST<zCProgMeshProto>(col2->parent->visual) ) {
				if ( !collision ) {
					// FIND() — pseudocode
					if ( FIND(pm.subMeshes, IT->material->matGroup == MAT_WOOD) ) {
						vob->SetCollDetStat(0);
						vob->SetCollDetDyn(0);
						vob->SetPhysicsEnabled(0);
						auto right = vob->trafoObjToWorld->GetRightVector();
						vob->Move(-right * 15.0);
					}
				}
			}
		}

		vob->GetRigidBody()->flags.1 = 1;
		collision = 1;
	}
}

void oCAIArrowBase::DoAI(zCVob *vob, int& vobDeleted)
{
	if ( trail ) {
		trailTime -= ztimer.frameTimeFloat;
		if ( trailTime <= 0.0 ) {
			trailTime = TRAIL_STRIP_SEGMENT_TIME;
			auto pos = vob->GetPositionWorld();
			trail->AddSegment(pos);
		}
	}

	vobDeleted = 0;
	if ( collision ) {
		collision = 0;
		if ( auto it = zDYNAMIC_CAST<oCItem>(vob) ) {
			if (it->effectVob)
				vob->effectVob->SetLevel(3, 1); // doesn't make much sense
			// maybe it is not oCVisualFX but a different type?
		}
		if ( startDustFX ) {
			vob->EndMovement(1);

			auto pfx = new zCParticleFX;
			auto pfxvob = new zCVob;

			pfxvob->SetCollDetStat(0);
			pfxvob->SetCollDetDyn(0);
			pfxvob->SetPositionWorld(vob->GetPositionWorld());
			pfxvob->SetVisual(pfx);
			pfx->SetEmitter("PFX_DUST");
			emitter->bs_1 *= 0.5; // unknown values
			emitter->bs_2 *= 0.5;
			pfx->CreateParticles();
			vob->homeWorld->AddVob(pfxvob);
			pfx->Release();
			pfxvob->Release();
		}
	}

	if ( timeLeft != -1.0 ) {
		timeLeft -= ztimer.frameTimeFloat * 0.00005;
		if ( timeLeft <= 0.0 )
			vobDeleted = 1;
		vob->visualAlpha = 0.5 * (sin(timeLeft * PI - PI/2) + 1.0);
		vob->flags1.4 = 1;
	}

	if ( vobDeleted ) {
		if (trailVob)
			trailVob->homeWorld->RemoveVob(trailVob);
		Release(trailVob);
		Release(trail);
		if ( auto it = zDYNAMIC_CAST<oCItem>(vob) ) {
			if (it->effectVob)
				it->RemoveEffect();
		}
	}
}

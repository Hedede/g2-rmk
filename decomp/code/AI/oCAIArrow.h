class oCAIArrow : public oCAIArrowBase {
	Z_OBJECT(oCAIArrow);
public:
	oCAIArrow() = default;
	~oCAIArrow()
	{
		ClearUsedVobs();
	}

	void ClearUsedVobs()
	{
		ClearIgnoreCDVob();

		Release(arrow);
		Release(ownerNpc);
		Release(target);
	}
	void Archive(zCArchiver& arc) override
	{
		oCAIArrowBase::Archive(arc);

		if ( !arc.InProperties() && arc.InSaveGame() ) {
			arc.WriteObject("arrow", arrow);

			auto owner = ownerNpc;
			if ( oCNpc::dontArchiveThisNpc == owner )
				owner = nullptr;
			arc.WriteObject("ownerNpc", owner);
		}
	}
	void Unarchive(zCArchiver& arc) override
	{
		oCAIArrowBase::Unarchive(arc);
		if ( !arc.InProperties() && arc.InSaveGame() ) { {
			ClearUsedVobs();
			arrow = arc.ReadObject("arrow", 0);
			ownerNpc = arc.ReadObject("ownerNpc", 0);
		}
	}

	void DoAI(zCVob *vob, int& vobDeleted) override
	{
		oCAIArrowBase::DoAI(vob, vobDeleted);
		if (vobDeleted) {
			deleted = 1;
			vob->SetAI(0);
		}
	}

	int CanThisCollideWith(zCVob *vob) override
	{
		if (ownerNpc->IsAPlayer() || !target)
			return !ignoreCD.Search(vob);

		// basically, don't collide with non-target
		// npcs to avoid friendly fire
		if (auto npc = zDYNAMIC_CAST<oCNpc>(vob))
			return npc == target || npc->IsMonster();
		return true;
	}

	void ReportCollisionToAI(zCCollisionReport const& colReport) override;

	void SetTarget(zCVob* vob)
	{
		Set(target, vob);
	}
private:
	zCVob *arrow    = nullptr;
	oCNpc *ownerNpc = nullptr;
	int deleted     = 0;
	zCVob *target   = nullptr;
};

//------------------------------------------------------------------------------
//_ulf/oAiShoot.cpp
void  oCAIArrow::ReportCollisionToAI(zCCollisionReport const& rep)
{
	collision = 0;
	zCVob* hitVob = 0;
	oCNpc* hitNpc = 0;
	oCNpc* selfNpc = 0;
	if (rep->col1->GetCollObjClass() == &zCCollObjectProjectile::s_oCollObjClass ) {
		arrow->StartCollisionSound(rep);
		if (auto co = dynamic_cast<zCCollObjectCharacter*>(rep->col2)) {
			hitVob = co->parent;
			if (hitVob) {
				ignoreCD.Insert(co->parent);
				hitVob->AddRef();
			}
		}

		hasHit = 1;
		if (arrow && hitVob) {
			hitNpc = zDYNAMIC_CAST<oCNpc>(hitVob);
			selfNpc = zDYNAMIC_CAST<oCNpc>(ownerNpc);

			if ( hitNpc ) {
				if ( hitNpc == selfNpc)
					return;

				auto dist = DIST(selfNpc, hitNpc);
				auto vm = selfNpc->GetWeaponMode();
				auto hc = selfNpc->GetHitChance(vm == FMODE_BOW ? NPC_TALENT_BOW : NPC_TALENT_CBOW);
				static int initonce = 0;
				static int ranged_mindist = 1000;
				static int ranged_maxdist = 10000;
				if (!initonce) {
					auto sym1 = zparser.GetSymbol("RANGED_CHANCE_MINDIST");
					if (sym1)
						sym1->GetSymbol(&ranged_mindist, 0);
					auto sym2 = zparser.GetSymbol("RANGED_CHANCE_MAXDIST");
					if (sym2)
						sym2->GetSymbol(&ranged_maxdist, 0);
				}

				// TODO: also add gothic1 behavior
				float chance;
				if ( dist <= ranged_mindist )
					chance = (hc - 100.0) * (dist / ranged_mindist) + 100.0;
				else
					chance = hc - hc * (dist / ranged_maxdist);

				zINFO(9, "C: Arrow could hit " + hitNpc.GetObjectName() + " calculated a final damagechance of " + chance + " percent"); //1128

				if ( rand() % 100 >= chance ) {
					hasHit = 0;
				} else {
					oCNpc::oSDamageDescriptor damDescr;
					damDescr.offender = selfNpc; // was "source_mb"
					damDescr.mult = 1;
					damDescr.source_mb = arrow;  // "alsosource"
					damDescr.damage = arrow->GetFullDamage();
					damDescr.pos1.x = rep->__contactPoint.x;
					damDescr.pos1.y = rep->__contactPoint.y;
					damDescr.pos1.z = rep->__contactPoint.z;
					damDescr.pos2.x = rep->__contactPoint.x;
					damDescr.pos2.y = rep->__contactPoint.y;
					damDescr.pos2.z = rep->__contactPoint.z;
					damDescr.source_npc = selfNpc;
					damDescr.weapon = arrow;
					damDescr.SetFXHit(0);
					damDescr.spellType = -1;
					damDescr.spellLevel = -1;
					damDescr.mult = 1; // typo not on my part
					damDescr.mode = 4;
					damDescr.type = DAM_POINT;

					auto msg = new oCMsgDamage(0, damDescr);
					hitNpc->GetEM(0)->OnMessage(msg);
					damDescr.Release();
				}

				if ( vob && hasHit )
					oCNpc::AssessFightSound_S(ownerNpc, vob->GetPositionWorld(), hitNpc);
			} else {
				if ( ownerNpc == hitVob )
					return;
				auto dmg = arrow->GetFullDamage();
				auto tp  = arrow->GetDamageTypes();
				hitVob->GetEM(0)->OnDamage(v41, arrow, ownerNpc, dmg, tp, arrow->GetPositionWorld(););
			}
		}


		if (arrow)
			AddIgnoreCDVob(arrow);
		deleted = 1;
		oCAIArrowBase::ReportCollisionToAI(rep);
	}
}

void oCAIArrow::SetupAIVob(oCItem *inVob, zCVob *shootVob, zCVob *target)
{
	oCAIArrowBase::ClearIgnoreCDVob();

	Release(arrow);
	Release(ownerNpc);
	Release(this->target);
	SetTarget(target);
	Set(vob, inVob);

	collision = 0;

	if (auto npc = zDYNAMIC_CAST<oCNpc>(shootVob))
		Set(ownerNpc, shootVob);
	if (auto itm = zDYNAMIC_CAST<oCItem>(inVob))
		Set(arrow, inVob);

	auto handTrafo = shootVob->GetTrafoModelNodeToWorld("ZS_RIGHTHAND");

	if (auto itm = zDYNAMIC_CAST<oCItem>(inVob)) {
		if (itm->effectVob)
			itm->effectVob->SetLevel(2,1);
	}

	AddIgnoreCDVob(shootVob);
	inVob->SetSleeping(0);
	inVob->SetPhysicsEnabled(1);
	inVob->SetTrafoObjToWorld(handTrafo);
	handTrafo.PostRotateY(-90.0);
	auto atvec = handTrafo.GetAtVector();
	atvec *= 2000.0;

	inVob->GetRigidBody()->SetVelocity(atvec);
	inVob->GetRigidBody()->flags.1 = 0;
	inVob->SetCollDetStat(1);
	inVob->SetCollDetDyn(1);

	if ( target ) {
		auto pos1 = inVob->GetPositionWorld();
		auto pos2 = target->GetPositionWorld();

		auto dir = pos2 - pos1;
		dir.Normalize();
		inVob.SetHeadingAtWorld(dir);


		auto trafo = inVob->trafoObjToWorld;
		trafo.PostRotateY(90.0);
		inVob->SetHeadingAtWorld(trafo);
		inVob->GetRigidBody()->SetVelocity(dir * 2000.0);
	}

	inVob->SetCollisionClass(&zCCollObjectProjectile::s_oCollObjClass);
	arow->SetFlag(ITEM_NFOCUS);
	CreateTrail(inVob);
}

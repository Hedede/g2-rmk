struct oCMagFrontier {
	oCMagFrontier() = default;
	~oCMagFrontier()
	{
		DisposeWarningFX();
		DisposeShootFX();
		npc = nullptr;
	}

	void DisposeWarningFX()
	{
		if ( warningFX ) {
			warningFX->Stop(1);
			Release(warningFX);
		}
		flags.isWarning = 0;
	}
	void DisposeShootFX()
	{
		if ( shootFX ) {
			shootFX->Stop(1);
			Release(shootFX);
		}
		flags.isShooting = 0;
	}

	SetNPC(oCNpc *npc)
	{
		this->npc = npc;
	}
	oCVisualFX*     warningFX = nullptr;
	oCVisualFX*     shootFX   = nullptr;
	oCNpc*          npc       = nullptr;

	struct {
		unsigned isWarning  : 1;
		unsigned isShooting : 1;
	} flags;
};

// I added missing code from Gothic1 (see comments below)
void oCMagFrontier::DoCheck()
{
	if (npc != oCNpc::player)
		return;
	if (zCCamera::activeCam->connectedVob->homeWorld->csPlayer->GetPlayingGlobalCutscene())
		return;
	if (!ogame || !gameMan)
		return;
	auto world = ogame->GetGameWorld();
	if (!world)
		return;
	auto worldName = world->GetWorldName();
	if (!npc)
		return;
	if ( npc->GetAttribute(ATR_HITPOINTS) <= 0 )
		return;
	zVEC3 playerProjPos = npc->GetPositionWorld();
	zVEC3 startPoint;
	float distToBarriere = 60000.0;

	// Gothic1 version
	if ( worldName == "WORLD" ) {
		GetDistance(playerProjPos, &distToBarriere, &startPoint);

		if ( world->csPlayer->GetPlayingGlobalCutscene() )
			return;

		if ( distToBarriere > 1200.0 || distToBarriere < 0.0 ) {
			DisposeShootFX();
			DisposeWarningFX();
		} else {
			float d = (1200.0 - distToBarriere) * 10.0 / (1200.0 - 650.0);
			int level = (int)d;
			if ( !level )
				level = 1;
			DoWarningFX(level);
			if ( distToBarriere > 650.0 || distToBarriere < 0.0 ) {
				DisposeShootFX();
			} else {
				DoShootFX(startPoint);
			}
		}
		return;
	}

	// Gothic2 version
	if (worldName == "NEWWORLD")
		GetDistanceNewWorld(playerProjPos, &distToBarriere, &startPoint);
	else if (worldName == "DRAGONISLAND")
		GetDistanceDragonIsland(playerProjPos, &distToBarriere, &startPoint);
	else if (worldName == "ADDONWORLD")
		GetDistanceAddonWorld(playerProjPos, &distToBarriere, &startPoint);
	else
		return;

	if ( world->csPlayer->GetPlayingGlobalCutscene() )
		return;

	bool debug = ogame && ogame->GetTextView() && ogame->game_testmode;
	if ( distToBarriere > 3000.0 || distToBarriere < 0.0 ) {
		if ( oceanWarnLevel > 0 ) {
			if ( oceanWarnSFX && zsound && zsound->IsSoundActive(&oceanWarnSoundHandle) ) {
				zsound->StopSound(&oceanWarnSoundHandle);
				oceanWarnSoundHandle = 0;
			}
			if ( debug ) {
				ogame->GetTextView()->Printwin("Well, swim home to mommy.");
			}
		}
		oceanWarnLevel = 0;
		return;
	}

	if ( distToBarriere > 1500.0 ) {
		if ( oceanWarnLevel == 1 ) {
			oceanWarnLevel = 1;
			return;
		}

		if ( oceanWarnLevel == 0 ) {
			if (zsound && !oceanWarnSFX) {
				oceanWarnSFX = zsound->LoadSoundFXScript("OCEAN_WARN");
			}
		}

		if ( oceanWarnSFX )
			oceanWarnSoundHandle = zsound->PlaySound(oceanWarnSFX, 0);

		if ( debug )
			ogame->GetTextView()->Printwin("Piranhas are watching you.");

		oceanWarnLevel = 1;
		return;
	}

	if ( debug ) {
		if ( oceanWarnLevel != 2 ) {
			ogame->GetTextView()->Printwin("Piranhas crunching your boots.");
			oceanWarnLevel = 2;
			return;
		}
	} else {
		npc->SetAttribute(ATR_HITPOINTS, 0);
		npc->DoDie(0);
		gameMan.PlayVideoEx("FishFood.bik");
	}
	oceanWarnLevel = 2;
}

void oCMagFrontier::DoWarningFX(int level)
{
	if (!flags.isShooting) {
		if ( flags.isWarning ) {
			warningFX->SetLevel(level, 0);
		} else {
			flags.isWarning = 1;
			warningFX = new oCVisualFX();
			warningFX->SetByScript("BARRIERE_PC_WARNING");
			warningFX->SetPositionWorld(npc->GetPositionWorld());
			npc->homeWorld->AddVob(warningFX);
			warningFX->Init(npc,0,0);
			warningFX->SetLevel(1, 0);
		}
	}
}

void  oCMagFrontier::DoShootFX(zVEC3 const& startPoint)
{
	if ( !flags.isShooting ) {
		flags.isShooting = 1;

		shootFX = new oCVisualFX();
		shootFX->SetByScript("BARRIERE_PC_SHOOT");

		auto playerPos = npc->GetPositionWorld();
		shootFX->SetPositionWorld(playerPos);

		shootFX->Init(npc, 0, 0);
		shootFX->Cast(1);

		auto dist = playerPos - barrierPos;
		auto dir = dist.Normalized();
		npc->GetEM()->OnDamage(npc, shootFX, 5.0, DAM_MAGIC|DAM_BARRIER, dir);

		dir *= 1000.0;
		dir.z = 0;

		zVEC3 lpos{0, 1000, 0};

		lpos += dir;
		lpos += npcPos;

		StartLightningAtPos(lpos, npcPos);
	}
}

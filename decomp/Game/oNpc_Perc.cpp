int oCNpc::AssessRemoveWeapon_S(oCNpc *other)
{
	oCNpc::CreatePassivePerception(PERC_ASSESSREMOVEWEAPON, this, 0);
	return 1;
}

int oCNpc::AssessTheft_S(oCNpc* other)
{
	CreatePassivePerception(PERC_ASSESSTHEFT, other, this);
	return 1;
}

int oCNpc::AssessFakeGuild_S()
{
	if ( IsSelfPlayer() ) {
		CreatePassivePerception(PERC_ASSESSFAKEGUILD, this, 0);
		return 1;
	}

	return 0;
}

int oCNpc::AssessCaster_S(oCSpell* spell)
{
	CreatePassivePerception(PERC_ASSESSCASTER, this, 0);
	return 1;
}

int oCNpc::AssessEnterRoom_S(oCNpc *this)
{
	CreatePassivePerception(PERC_ASSESSENTERROOM, this, 0);
	return 1;
}

int oCNpc::AssessDefeat_S(zCVob* commiter)
{
	CreatePassivePerception(PERC_ASSESSDEFEAT, commiter, this);
	return 1;
}

int oCNpc::AssessMurder_S(zCVob* commiter)
{
	CreatePassivePerception(PERC_ASSESSMURDER, commiter, this);
	return 1;
}

int oCNpc::IsInFightMode_S(int fmode)
{
	switch (GetWeaponMode()) {
	case FMODE_NONE:
		return fmode == FMODE_NONE;
	case FMODE_FIST:
		return fmode == FMODE_FIST;
	case FMODE_MELEE:
		return fmode >= FMODE_MELEE &&
		       fmode <  FMODE_FAR;
	case FMODE_FAR:
		return fmode == FMODE_BOW ||
		       fmode == FMODE_CBOW;
	case FMODE_MAGIC:
		return fmode == FMODE_MAGIC;
	default:
		return 0;
	}
}

bool oCNpc::GetScriptInstance(zSTRING*& name, int& index)
{
	name = GetSymbol(instanz)->name;
	index = instanz;
	return 1;
}


bool oCNpc::SetByScriptInstance(zSTRING const* name, int index)
{
	if ( name )
		index = zparser.GetIndex(name);
	InitByScript(index, 0);
	return 1;
}

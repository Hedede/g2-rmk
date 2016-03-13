int oCNpc::IsInFightMode_S(int smode)
{
	if ( fmode < 0 || fmode >= 8 )
		fmode = 0;

	switch (smode) {
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

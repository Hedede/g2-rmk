class oCTouchDamage : zCTouchDamage {
	Z_OBJECT(oCTouchDamage);
public:
	virtual ~oCTouchDamage() = default;

	const char* GetDamageTypeArcEnum(unsigned long idx) override
	{
		switch ( idx ) {
		case DAM_INDEX_BARRIER: return "Barrier";
		case DAM_INDEX_BLUNT:   return "Blunt";
		case DAM_INDEX_EDGE:    return "Edge";
		case DAM_INDEX_FIRE:    return "Fire";
		case DAM_INDEX_FLY:     return "Fly";
		case DAM_INDEX_MAGIC:   return "Magic";
		case DAM_INDEX_POINT:   return "Point";
		case DAM_INDEX_FALL:    return "Fall";
		}
		return nullptr;
	}

};

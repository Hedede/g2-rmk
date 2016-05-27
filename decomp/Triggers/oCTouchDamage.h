class oCTouchDamage : zCTouchDamage {
	Z_OBJECT(oCTouchDamage);
public:
	virtual ~oCTouchDamage() = default;

	virtual void GetDamageTypeArcEnum(ulong);
};

class zCTouchDamage : public zCEffect {
	Z_OBJECT(zCTouchDamage);
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~zCTouchDamage() = default;

	virtual void OnTouch(zCVob *);
	virtual void OnUntouch(zCVob *);
	virtual void OnTimer();
	virtual void SetVisual(zCVisual *);

	virtual void GetDamageTypeArcEnum(uint32_t damType) const
	{
		return damType == 0 ? "damageTypeDefault" : 0;
	}

private:
	float damage;
	int damageType;
	float damageRepeatDelaySec;
	float damageVolDownScale;
	int damageCollType;
};

void zCTouchDamage::Archive(zCArchiver& arc)
{
	zCEffect::Archive(arc);

	arc.WriteGroupBegin("TouchDamage");
	arc.WriteFloat("damage", damage);
	arc.WriteGroupBegin("DamageType");

	char const* damType;
	for (int i = 0; damType = GetDamageTypeArcEnum(i); ++i)
		arc.WriteBool(damType, (damageType & (1 << i)) != 0);

	arc.WriteGroupEnd("DamageType");
	arc.WriteFloat("damageRepeatDelaySec", damageRepeatDelaySec);
	arc.WriteFloat("damageVolDownScale", damageVolDownScale);
	arc.WriteEnum("damageCollType", "NONE;BOX;POINT", damageCollType);
	arc.WriteGroupEnd("TouchDamage");
}

void zCTouchDamage::Unarchive(zCArchiver& arc)
{
	zCEffect::Unarchive(arc);

	arc.ReadFloat("damage", damage);

	char const* damType;
	int i;
	for (i = 0; damType = GetDamageTypeArcEnum(i); ++i )
		damageType |= (arc.ReadBool(damType) << i);

	if (!i)
		damageType = 1;

	arc.ReadFloat("damageRepeatDelaySec", damageRepeatDelaySec);
	arc.ReadFloat("damageVolDownScale", damageVolDownScale);

	damageCollType = arc.ReadEnum("damageCollType");
}

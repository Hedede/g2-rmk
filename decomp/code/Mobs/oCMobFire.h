class oCMobFire : public oCMobInter {
	Z_OBJECT(oCMobFire);
public:
	virtual void Archive(zCArchiver&);
	virtual void Unarchive(zCArchiver&);
	virtual ~oCMobFire();
	virtual void OnTrigger(zCVob *,zCVob *);
	virtual void OnUntrigger(zCVob *,zCVob *);
	virtual void OnDamage(zCVob *,zCVob *,float,int,zVEC3 const &);
	virtual void OnEndStateChange(oCNpc *,int,int);
	virtual void PreSave();
	virtual void PostSave();
private:
	zSTRING fireSlot;        // z.B: "BIP01 FIRE"
	zSTRING fireVobtreeName; // z.B. "FIRETREE_MEDIUM.ZEN"
	
	zCVob* fireVobtree;
};

void oCMobFire::Archive(zCArchiver& arc)
{
	oCMobInter::Archive(arc);

	arc.WriteGroupBegin("Fire");
	arc.WriteString("fireSlot", fireSlot);
	arc.WriteString("fireVobtreeName", fireVobtreeName);
	arc.WriteGroupEnd("Fire");

	if (arc.InSaveGame()) {
	}
}

void oCMobFire::Unarchive(zCArchiver& arc)
{
	oCMobInter::Unarchive(arc);

	arc.ReadString("fireSlot", fireSlot);
	arc.ReadString("fireVobtreeName", fireVobtreeName);
}

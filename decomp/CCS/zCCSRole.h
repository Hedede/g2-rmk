class zCCSRole : public zCObject{
	Z_OBJECT(zCCSRole);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual ~zCCSRole();
private:
	zSTRING roleName;
	bool mustBeAlive;
	int roleType;
	zCVob *integratedVob;
};

void zCCSRole::Archive(zCArchiver& arc)
{
	arc.WriteBool("mustBeAlive", mustBeAlive);
	arc.WriteString("roleName", roleName);
	arc.WriteEnum("roleType", "CSRrequired;CSRdelayable;CSRnonrequired", roleType);
	if ( !arc.InProperties() )
		arc.WriteObject_cp("integratedVob", integratedVob);
}

void zCCSRole::Unarchive(zCArchiver& arc)
{
	arc.ReadBool("mustBeAlive", mustBeAlive);
	arc.ReadString("roleName", roleName);
	arc.ReadEnum("roleType", roleType);

	if ( !arc.InProperties() ) {
		auto obj = arc.ReadObject("integratedVob", 0);
		integratedVob = dynamic_cast<zCVob>(obj);

	}
}

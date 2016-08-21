class oCObjectGenerator : public zCVob {
public:
	virtual void Archive(zCArchiver& arc)
	{
		zCVob::Archive(arc);
		arc.WriteString("objectName", objectName);
		arc.WriteFloat("objectSpeed", objectSpeed);
	}
	virtual void Unarchive(zCArchiver& arc)
	{
		zCVob::Unarchive(arc);
		arc.ReadString("objectName", objectName);
		arc.ReadFloat("objectSpeed", objectSpeed);
	}
	virtual ~oCObjectGenerator();
	virtual void OnTrigger(zCVob *,zCVob *);
private:
	float objectSpeed;
	zSTRING objectName;
};

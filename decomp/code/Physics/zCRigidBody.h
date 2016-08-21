class zCRigidBody {
public:
	void Archive(zCArchiver& arc)
	{
		arc.WriteVec3("vel", vel);
		arc.WriteByte("mode", flags.mode);
		arc.WriteBool("gravOn", flags.gravOn);
		arc.WriteFloat("gravScale", gravScale);
		arc.WriteVec3("slideDir", slideDir);
	}

	void Unarchive(zCArchiver& arc)
	{
		vel = arc.ReadVec("vel");
		force = {};
		vec1 = (1.0 / float2) * vel;

		flags.mode = arc.ReadByte("mode");
		flags.gravOn = arc.ReadBool("gravOn");

		arc.ReadFloat("gravScale", gravScale);
		arc.ReadVec3("slideDir", slideDir);
	}

	void ApplyForce(zVEC3 const& forceVec, zVEC3 const& pos)
	{
		force += forceVec;
	}

private:
	struct {
		uint32_t gravOn  : 1;
		uint32_t unused1 : 1;
		uint32_t mode    : 1;
		uint32_t unused2 : 4;
		uint32_t unknown : 1;
	} flags;
};

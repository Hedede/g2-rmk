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

	void ApplyForceCM(zVEC3 const& forceVec)
	{
		force += forceVec;
	}

	void ApplyForce(zVEC3 const& forceVec, zVEC3 const& pos)
	{
		ApplyForceCM( forceVec );
	}

	// Apply impulse to the center of mass
	void ApplyImpulseCM(const zVEC3& impulse)
	{
		this->impulse += impulse;
		vel = invMass * this->impulse;
	}

	void ApplyImpulse(const zVEC3& impulse, const zVEC3& point)
	{
		ApplyImpulseCM( impulse );
	}

	void RunSimulation()
	{
		auto deltaTime = ztimer.frameTimeFloat * 0.001;
		if ( deltaTime > 0.0 )
			Integrate(deltaTime);
	}

	void Integrate(float deltaTime);

	void ComputeForceTorque(float t) {}

	enum zTRigidBodyMode { };

	void SetMode(const zTRigidBodyMode& m)
	{
		rbFlags.mode = m;
	}

	void StopRotation();
	void StopTranslation();
	void StopTransRot();

private:
	float mass;
	float invMass;
	zMAT3 mat1;
	zMAT3 mat2;
	zVEC3 vec0;
	zMAT3 mat3;
	zVEC3 impulse;
	zVEC3 vec2;
	zMAT3 mat4;
	zVEC3 vel;
	zVEC3 vec3;
	zVEC3 force;
	zVEC3 vec4;
	float gravScale;
	zVEC3 slideDir;
	float slideAngle;

	struct {
		uint32_t gravOn  : 1; // 1
		uint32_t unused1 : 1; // 2
		uint32_t mode    : 1; // 4
		uint32_t unused2 : 4;
		uint32_t unknown : 1;
	} flags;
};


void zCRigidBody::Integrate(float deltaTime)
{
	if ( rbFlags.unused2 )
		--rbFlags.unused2;
		// rbFlags = rbFlags ^ (rbFlags ^ ((rbFlags & 0xF8) - 1)) & 0x78;
	if ( rbFlags.unused1 ) {
		rbFlags.unused1 = 0;
		force.x = force.x;
		force.y = force.y + 1.0;
		force.z = force.z;
	} else if ( rbFlags.gravOn ) {
		if ( rbFlags.mode == 1 ) {
			zVEC3 impulse = vel * __mass;
	
			force -= impulse; 

			auto scaledMass = gravScale * __mass;
			auto slideForce = (slideDir.z + slideDir.x) * 0.0 + scaledMass * -981.0 * slideDir.y;

			force += slideForce * slideDir;
		} else {
			auto scaledMass = gravScale * __mass;

			force.x = force.x;
			force.y = scaledMass * -981.0 + force.y;
			force.z = force.z;
		}
	}

	ApplyImpulseCM( force * deltaTime ); // wasinlined

	vec0 += vel * deltaTime;

	force.x = 0.0;
	force.y = 0.0;
	force.z = 0.0;

	vec4.x = 0.0;
	vec4.y = 0.0;
	vec4.z = 0.0;
}

void zCRigidBody::StopTranslation()
{
	force.x = 0.0;
	force.y = 0.0;
	force.z = 0.0;
	impulse.x = 0.0;
	impulse.y = 0.0;
	impulse.z = 0.0;
	vel.x = 0.0;
	vel.y = 0.0;
	vel.z = 0.0;
}

void zCRigidBody::StopTransRot()
{
	StopRotation();
	StopTranslation();
	SetMode( 0 ); // rbFlags &= 0xFBu;
}

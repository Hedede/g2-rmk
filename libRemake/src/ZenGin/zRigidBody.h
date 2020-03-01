#pragma once

#include <ZenGin/zMAT3.h>

struct zCRigidBody
{
	void ApplyImpulseCM(const zVEC3& impulse)
	{
		this->impulse += impulse;
		vel = this->impulse * this->invMass;
	}

	void ApplyImpulse(const zVEC3& impulse, const zVEC3& point)
	{
		ApplyImpulseCM(impulse);
	}

	float mass;
	float invMass;
	zMAT3 mat1;
	zMAT3 mat2;
	zVEC3 vec0;
	zMAT3 mat3;
	zVEC3 impulse;
	zVEC3 vec2;
	int unk_1[9];
	zVEC3 vel;
	zVEC3 vec3;
	zVEC3 force;
	zVEC3 vec4;
	float gravScale;
	zVEC3 slideDir;
	int unk_4;
	char rbFlags;
};
CHECK_SIZE(zCRigidBody, 0x104);

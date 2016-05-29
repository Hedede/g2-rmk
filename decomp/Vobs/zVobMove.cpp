void zCVob::Move(float x, float y, float z)
{
	MoveWorld(x,y,z);
}

void zCVob::MoveWorld(float x, float y, float z)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto& trafo = collisionObject->trafoObjToWorld;
	trafo[0][2] += x;
	trafo[1][2] += y;
	trafo[2][2] += z;

	collisionObject->flags |= 1u;

	if (!movmode)
		EndMovement(1);
}

void zCVob::MoveLocal(float x, float y, float z)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto& trafo = collisionObject->trafoObjToWorld;

	auto mov = zVEC3{x,y,z} * trafo; // not sure
	MoveWorld(mov.x, mov.y, mov.z);

	if (!movmode)
		EndMovement(1);
}

void zCVob::RotateWorldX(float angle)
{
	RotateWorld({1,0,0}, angle);
}

void zCVob::RotateWorldY(float angle)
{
	RotateWorld({0,1,0}, angle);
}

void zCVob::RotateWorldZ(float angle)
{
	RotateWorld({0,0,1}, angle);
}

void zCVob::RotateLocalX(float angle)
{
	if ( angle != 0.0 ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		collisionObject->trafoObjToWorld.PostRotateX(angle);
		collisionObject->flags |= 2u;

		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::RotateLocalY(float angle)
{
	if ( angle != 0.0 ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		collisionObject->trafoObjToWorld.PostRotateY(angle);
		collisionObject->flags |= 2u;

		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::RotateLocalZ(float angle)
{
	if ( angle != 0.0 ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		collisionObject->trafoObjToWorld.PostRotateZ(angle);
		collisionObject->flags |= 2u;

		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::RotateLocal(zVEC3 const& axis, float angle)
{
	if ( angle != 0.0 ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		zMAT4 rot = Alg_Rotation3D(axis, angle)
		zMAT4 mat = collisionObject->trafoObjToWorld * rot;

		collisionObject->trafoObjToWorld = mat;
		collisionObject->flags |= 2u;

		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::RotateWorld(zVEC3 const& axis, float angle)
{
	if ( angle != 0.0 ) {
		int movmode = isInMovementMode;
		if (!movmode)
			BeginMovement();

		auto pos = collisionObject->trafoObjToWorld.GetTranslation();
		collisionObject->trafoObjToWorld.SetTranslation(0, 0, 0);

		zMAT4 rot = Alg_Rotation3D(axis, angle)
		zMAT4 mat = collisionObject->trafoObjToWorld * rot;

		collisionObject->trafoObjToWorld.SetTranslation(pos);
		collisionObject->flags |= 2u;

		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::SetCollDetStat(int val)
{
	if ( val && isInMovementMode == 2)
		zFAULT("D: zVob(zCVob::SetCollDetStat): cannot modify collision-flags while in movement-phase"); // 1002, dieter
	flags1.collDetectionStatic = val;
}

void zCVob::SetCollDetDyn(int val)
{
	if ( val && isInMovementMode == 2)
		zFAULT("D: zVob(zCVob::SetCollDetDyn): cannot modify collision-flags while in movement-phase"); // 1013
	flags1.collDetectionDynamic = val;
}

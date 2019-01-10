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

void zCVob::SetRotationWorld(zCQuat const& quat)
{
	int movmode = isInMovementMode;
	if ( !movmode )
		BeginMovement();

	quat.QuatToMatrix4(collisionObject->trafoObjToWorld);
	collisionObject->flags |= 2u;

	if ( !movmode )
		EndMovement(1);
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

void zCVob::ResetRotationsWorld()
{
	auto movmode = isInMovementMode;
	if ( !movmode )
		BeginMovement();

	auto pos = GetPositionWorld();
	collisionObject->trafoObjToWorld = zMAT4::s_identity;
	collisionObject->trafoObjToWorld.SetTranslation(pos); // didn't look up, was inlined

	collisionObject->flags |= 2u;

	if ( !movmode )
		EndMovement(1);
}

void zCVob::ResetXZRotationsWorld()
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto mat = collisionObject->trafoObjToWorld;

	zVEC3 v;
	v.x = mat[0][2] * 1000.0 + mat[0][3];
	v.y = mat[1][3];
	v.z = mat[2][2] * 1000.0 + mat[2][3];

	SetHeadingWorld( v );

	if (!movmode)
		EndMovement(1);
}

void zCVob::ResetRotationsLocal()
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	if ( HasParentVob() ) {
		auto parent = globalVobTreeNode->parent->Get();

		auto pos = collisionObject->trafoObjToWorld.GetTranslation();
		collisionObject->trafoObjToWorld = parent->trafoObjToWorld;
		collisionObject->trafoObjToWorld.SetTranslation(pos);
		collisionObject->flags |= 2u;
	} else {
		ResetRotationsWorld();
	}

	if (!movmode)
		EndMovement(1);
}

void zCVob::ResetXZRotationsLocal()
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	if ( HasParentVob() ) {
		auto parent = globalVobTreeNode->parent->Get();

		auto trafo = collisionObject->trafoObjToWorld;
		auto pTrafo = parent->trafoObjToWorld;

		auto mat = pTrafo.Inverse() * trafo;

		zVEC3 v;
		v.x = mat[0][2] * 1000.0 + mat[0][3];
		v.y = mat[1][3];
		v.z = mat[2][2] * 1000.0 + mat[2][3];

		v = pTrafo * v;

		SetHeadingWorld( v );
	} else {
		ResetXZRotationsWorld();
	}

	if (!movmode)
		EndMovement(1);
}

void zCVob::SetHeadingYLocal(zVEC3 const& targetPos)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject->trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtLocal(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}

	ResetXZRotationsLocal();
}

void zCVob::SetHeadingWorld(zVEC3 const& vec)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject->trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtWorld(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}
}

void zCVob::SetHeadingLocal(zVEC3 const& vec)
{
	int movmode = isInMovementMode;
	if (!movmode)
		BeginMovement();

	auto pos = collisionObject->trafoObjToWorld.GetTranslation();
	auto diff = pos - vec;
	if (diff.Length() > 0.0 ) {
		SetHeadingAtLocal(diff.Normalized()); // didn't care to look up
		if (!movmode)
			EndMovement(1);
	}
}


void zCVob::BeginMovement()
{
	if ( !isInMovementMode )
		return;

	if ( flags2.bInsideEndMovementMethod )
	{
		zWARNING( "D: VOB: illegal call of vob::BeginMovement() from a collision notification callback" ); //1461, dieter
	}
	else
	{
		CreateCollisionObject();
		ResetCollisionObjectState();

		auto movmode = isInMovementMode;
		flags2.8 = 0;
		SetInMovementMode( 1 );
	}
}

void zCVob::EndMovement(zCVob *this, int calcGround)
{
	if ( !isInMovementMode )
		return;

	if ( flags2.bInsideEndMovementMethod ) {
		zWARNING("D: VOB: illegal call of vob::EndMovement() from a collision notification callback"); // 1646, dieter
		return;
	}

	flags2.bInsideEndMovementMethod  = 1;
	if ( homeWorld ) {
		if ( collisionObject->flags & 3 ) {
			CheckAndResolveCollisions();
			zTBBox3D bbox;
			CalcDestinationBBox3DWorld(bbox);
			this->bbox3D = bbox;
			AdoptCollObjResults();
			SetInMovementMode(0);
			if ( homeWorld->ShouldAddThisVobToBsp(this) )
				homeWorld->bspTree.AdjustVob(this);

			if ( globalVobTreeNode )
			{
				for ( auto it = globalVobTreeNode->firstChild; it; it = it->next) {
					auto child = it->Get();
					child->BeginMovement(); //was inlined
					child->SetTrafoObjToWorld( trafoObjToWorld * child->GetPositionLocal() );// both calls were inlined
					child->EndMovement();
				}
			}

			if ( calcGround )
				CalcGroundPoly();

			// TODO: Recheck, might've made a mistake
			lightColorStatDirty = true;
			lightColorDynDirty = true;
			CheckAutoUnlink();
			if ( visual && calcGround )
				CalcWaterVob();
		}
		else
		{
			AdoptCollObjResults();
			SetInMovementMode(0);
		}
	} else {
		zTBBox3D bbox;
		CalcDestinationBBox3DWorld((struct zTBBox3D *)&msg.data);
		this->bbox3D = bbox;
		AdoptCollObjResults();
		SetInMovementMode(0);
		if ( globalVobTreeNode )
		{
			for ( auto it = globalVobTreeNode->firstChild; it; it = it->next) {
				auto child = it->Get();
				child->BeginMovement(); //was inlined
				child->SetTrafoObjToWorld( trafoObjToWorld * child->GetPositionLocal() ); // both calls were inlined
				child->EndMovement( calcGround ); 
			}
		}
	}
	flags2.bInsideEndMovementMethod  = 0;
}

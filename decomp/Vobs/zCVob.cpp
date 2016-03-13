void zCVob::ArchiveVerbose(zCArchiver& arc)
{
	arc.WriteGroupBegin("Internals");

	arc.WriteInt("pack", 0);

	arc.WriteString("presetName", vobPresetName ? *vobPresetName : ""_s);

	arc.WriteRawFloat("bbox3DWS", bbox3D, 24);

	arc.WriteRaw("trafoOSToWSRot", trafoObjToWorld.ExtractRotation());
	arc.WriteVec3("trafoOSToWSPos", trafoOSToWSPos.GetTranslation());
	arc.WriteGroupEnd("Internals");

	arc.WriteGroupBegin("Vob");

	arc.WriteString("vobName", GetObjectName());

	arc.WriteString("visual", visual ? visual->GetVisualName() : "");

	arc.WriteBool("showVisual", flags.showVisual);

	arc.WriteEnum("visualCamAlign", "NONE;YAW;FULL", visualCamAlign);
	arc.WriteEnum("visualAniMode", "NONE;WIND;WIND2", aniMode);

	arc.WriteFloat("visualAniModeStrength", aniModeStrength);
	arc.WriteFloat("vobFarClipZScale", vobFarClipZScale);

	arc.WriteBool("cdStatic", flags1.collDetectionStatic);
	arc.WriteBool("cdDyn", flags1.collDetectionDynamic);
	arc.WriteBool("staticVob", flags1.staticVob);
	arc.WriteEnum("dynShadow", "DS_NONE;DS_BLOB", flags1.castDynShadow);

	arc.WriteInt("zbias", zBias);
	arc.WriteBool("isAmbient", flags3.isAmbientVob);
	arc.WriteGroupEnd("Vob");

	arc.WriteObject("visual", visual);
	arc.WriteObject("ai", callback_ai);

	if ( arc.InSaveGame() ) {
		arc.WriteObject(eventManager);
		arc.WriteBool("physicsEnabled", flags1.physicsEnabled);
	}
}

void zCVob::ArchivePacked(zCArchiver& arc)
{
	char dataPacked[83];

	arc.WriteInt("pack", 1);

	aw::copy_raw(bbox3D, &dataPacked[0]);

	auto pos = trafoObjToWorld.GetTranslation();
	aw::copy_raw(pos[0][3], dataPacked[ 24 ]);
	aw::copy_raw(pos[1][3], dataPacked[ 28 ]);
	aw::copy_raw(pos[2][3], dataPacked[ 32 ]);

	auto rot = trafoObjToWorld.ExtractRotation();
	aw::copy_raw(rot[0][0], dataPacked[ 36 ]);
	aw::copy_raw(rot[0][1], dataPacked[ 40 ]);
	aw::copy_raw(rot[0][2], dataPacked[ 44 ]);
	aw::copy_raw(rot[1][0], dataPacked[ 48 ]);
	aw::copy_raw(rot[1][1], dataPacked[ 52 ]);
	aw::copy_raw(rot[1][2], dataPacked[ 56 ]);
	aw::copy_raw(rot[2][0], dataPacked[ 60 ]);
	aw::copy_raw(rot[2][1], dataPacked[ 64 ]);
	aw::copy_raw(rot[2][2], dataPacked[ 68 ]);

	dataPacked[72] = 0;

	dataPacked[72] |= flags1.showVisual             * 1;
	dataPacked[72] |= (visualCamAlign & 3)          * 2;
	dataPacked[72] |= (flags1.collDetectionStatic)  * 8;
	dataPacked[72] |= (flags1.collDetectionDynamic) * 16;
	dataPacked[72] |= (flags1.ignoredByTraceRay)    * 32;
	dataPacked[72] |= (flags1.castDynShadow)        * 64;

	bool has_preset  = vobPresetName && vobPresetName->Length() > 0;
	bool has_vobname = !GetObjectName().IsEmpty();
	bool has_visname = !GetVisualName().IsEmpty();
	bool has_visual  = visual       != 0;
	bool has_ai      = callback_ai  != 0;
	bool has_em      = eventManager != 0;

	dataPacked[73]  = 0;
	dataPacked[73] |= has_preset   * 0x01;
	dataPacked[73] |= has_vobname  * 0x02;
	dataPacked[73] |= has_visname  * 0x04;
	dataPacked[73] |= has_visual   * 0x08;
	dataPacked[73] |= has_ai       * 0x10;
	dataPacked[73] |= has_em       * 0x20;

	dataPacked[74]  = 0;
	dataPacked[74] |= (aniMode & 3)       * 0x01;
	dataPacked[74] |= (zBias & 0x1F)      * 0x02;
	dataPacked[74] |= flags3.isAmbientVob * 0x40;

	aw::copy_raw(aniModeStrength, &dataPacked[75]);
	aw::copy_raw(vobFarClipZScale, &dataPacked[79]);

	arc.WriteRaw("dataRaw", dataPacked, 83);

	if ( dataPacked[73] & 1 )
		arc.WriteString("presetName", *vobPresetName);
	if ( dataPacked[73] & 2 )
		arc.WriteString("vobName", GetObjectName());
	if ( dataPacked[73] & 4 )
		arc.WriteString("visual", GetVisualName());
	if ( dataPacked[73] & 8 )
		arc.WriteObject("visual", visual);
	if ( dataPacked[73] & 0x10 )
		arc.WriteObject("ai", callback_ai);
	if ( arc.InSaveGame() && dataPacked[73] & 0x20 )
		arc.WriteObject(eventManager);

}

void zCVob::UnarchiveVerbose(zCArchiver& arc)
{
	arc.ReadString("presetName");

	arc.ReadRawFloat("bbox3DWS", bbox3D, 24);

	zMAT3 rot;
	zVEC3 pos;
	arc.ReadRaw("trafoOSToWSRot", rot, 36);
	arc.ReadVec3("trafoOSToWSPos", pos);

	trafoObjToWorld.SetRotation(rot);
	trafoObjToWorld.SetTranslation(pos);

	if (!homeWorld) {
		if ( trafo )
			trafo = trafoObjToWorld;
	}

	auto name = arc.ReadString("vobName");

	if ( homeWorld )
		homeWorld->RemoveVobHashTable(this);

	SetObjectName(name);

	if ( homeWorld )
		homeWorld->InsertVobHashTable(this);

	auto visName = arc.ReadString("visual");
	if (visName.Length() > 0) {
		auto tmp = zCMaterial::s_matUsageDefault;
		if ( this->type == 3 )
			zCMaterial::s_matUsageDefault = 0;

		SetVisual(visName);
		zCMaterial::s_matUsageDefault = tmp;

		auto pfx = zDYNAMIC_CAST<zCParticleFX>(visual);
		if ( pfx ) {
			bbox3D.mins[0] = trafoObjToWorld[0][3];
			bbox3D.mins[0] = trafoObjToWorld[0][3];
			bbox3D.mins[1] = trafoObjToWorld[1][3];
			bbox3D.maxs[1] = trafoObjToWorld[1][3];
			bbox3D.mins[2] = trafoObjToWorld[2][3];
			bbox3D.maxs[2] = trafoObjToWorld[2][3];
		}
	}


	flags1.showVisual = arc.ReadBool("showVisual");

	visualCamAlign = arc.ReadEnum("visualCamAlign");
	aniMode = arc.ReadEnum("visualAniMode");

	aniModeStrength  = arc.ReadFloat("visualAniModeStrength");
	vobFarClipZScale = arc.ReadFloat("vobFarClipZScale");

	SetCollDetStat(arc.ReadBool("cdStatic"));
	SetCollDetDyn(arc.ReadBool("cdDyn"));

	flags1.staticVob = arc.ReadBool("staticVob");
	flags1.dynShadow = arc.ReadEnum("dynShadow");

	arc.ReadInt("zbias", zBias);

	flags3.isAmbientVob = arc.ReadBool("isAmbient");

	auto obj1 = arc.ReadObject("visual", visual);
	auto obj2 = arc.ReadObject("ai", callback_ai);

	if (!visual) {
		SetVisual(static_cast<zCVisual*>(obj1));

		obj1->Release();
	}

	if ( !callback_ai ) {
		if (obj2) {
			callback_ai = obj2;
			obj2->AddRef();
			obj2->Release();
		}
	}


	if ( visual ) {
		if (zDYNAMIC_CAST<zCParticleFX>(visual))
			visual = zDYNAMIC_CAST<zCParticleFX>(visual);
	}

	if ( arc.InSaveGame() ) {
		eventManager = zfactory->CreateEventManager(this);
		auto obj = arc.ReadObject(eventManager);
		if (!obj)
			Release(eventManager);

		eventManager = static_cast<zCEventManager*>(obj);

		SetPhysicsEnabled(arc.ReadBool("physicsEnabled"));
	}
}

void zCVob::UnarchivePacked(zCArchiver& arc)
{
	char dataPacked[83];

	arc.ReadRaw("dataRaw", dataPacked, 83);

	memcpy(&bbox3D, dataPacked, sizeof(bbox3D));

	zMAT4 mat;

	mat[0][3] = aw::reinterpret<float>(dataPacked[ 24 ]);
	mat[1][3] = aw::reinterpret<float>(dataPacked[ 28 ]);
	mat[2][3] = aw::reinterpret<float>(dataPacked[ 32 ]);

	mat[0][0] = aw::reinterpret<float>(dataPacked[ 36 ]);
	mat[0][1] = aw::reinterpret<float>(dataPacked[ 40 ]);
	mat[0][2] = aw::reinterpret<float>(dataPacked[ 44 ]);
	mat[1][0] = aw::reinterpret<float>(dataPacked[ 48 ]);
	mat[1][1] = aw::reinterpret<float>(dataPacked[ 52 ]);
	mat[1][2] = aw::reinterpret<float>(dataPacked[ 56 ]);
	mat[2][0] = aw::reinterpret<float>(dataPacked[ 60 ]);
	mat[2][1] = aw::reinterpret<float>(dataPacked[ 64 ]);
	mat[2][2] = aw::reinterpret<float>(dataPacked[ 68 ]);

	trafoObjToWorld = mat;

	if (!homeWorld) {
		if ( trafo )
			trafo = trafoObjToWorld;
	}

	auto flags_pack1 = aw::reinterpret<int>(dataPacked[72]);
	auto flags_pack2 = aw::reinterpret<int>(dataPacked[73]);
	auto flags_pack3 = aw::reinterpret<int>(dataPacked[74]);

	if ( flags_pack2 & 1 ) {
		arc.ReadString("presetName");
	}

	if ( flags_pack2 & 2 ) {
		auto name = arc.ReadString("vobName");

		if ( homeWorld )
			homeWorld->RemoveVobHashTable(this);

		SetObjectName(name);

		if ( homeWorld )
			homeWorld->InsertVobHashTable(this);
	}

	if ( flags_pack2 & 4 ) {
		auto visName = arc.ReadString("visual");
		if (visName.Length() > 0) {
			auto tmp = zCMaterial::s_matUsageDefault;
			if ( this->type == 3 )
				zCMaterial::s_matUsageDefault = 0;

			SetVisual(visName);
			zCMaterial::s_matUsageDefault = tmp;

			auto pfx = zDYNAMIC_CAST<zCParticleFX>(visual);
			if ( pfx ) {
				bbox3D.mins[0] = trafoObjToWorld[0][3];
				bbox3D.mins[0] = trafoObjToWorld[0][3];
				bbox3D.mins[1] = trafoObjToWorld[1][3];
				bbox3D.maxs[1] = trafoObjToWorld[1][3];
				bbox3D.mins[2] = trafoObjToWorld[2][3];
				bbox3D.maxs[2] = trafoObjToWorld[2][3];
			}
		}
	}

	flags1.showVisual        = (flags_pack1 >> 0) & 1;
	visualCamAlign           = (flags_pack1 >> 1) & 3;
	SetCollDetStat           ( (flags_pack1 >> 3) & 1);
	SetCollDetDyn            ( (flags_pack1 >> 4) & 1);
	flags1.ignoredByTraceRay = (flags_pack1 >> 5) & 1;
	flags1.castDynShadow     = (flags_pack1 >> 6) & 3;

	aniMode             = (flags_pack3 >> 0) & 3;
	zBias               = (flags_pack3 >> 2) & 0x1F;
	flags3.isAmbientVob = (flags_pack3 >> 7) & 1;

	aniModeStrength  = aw::reinterpret<int>(dataPacked[75]);
	vobFarClipZScale = aw::reinterpret<int>(dataPacked[79]);

	if ( flags_pack2 & 8 ) {
		auto obj = arc.ReadObject("visual", visual);
		if (!visual) {
			SetVisual(static_cast<zCVisual*>(obj));

			obj->Release();
		}
	}

	if ( flags_pack2 & 0x10 ) {
		auto obj = arc.ReadObject("ai", callback_ai);
		if ( !callback_ai ) {
			if (obj) {
				callback_ai = obj;
				obj->AddRef();
				// in assembly it looks like Release()
				// but uses inc instead of dec
			}
		}
	}

	if ( visual ) {
		if (zDYNAMIC_CAST<zCParticleFX>(visual))
			visual = zDYNAMIC_CAST<zCParticleFX>(visual);
	}

	if ( arc.InSaveGame() ) {
		if ( flags_pack2 & 0x20 ) {
			if ( eventManager ) {
				eventManager = arc.ReadObject(eventManager);
			} else {
				eventManager = zfactory->CreateEventManager(this);
				auto obj = arc.ReadObject(eventManager);
				eventManager = static_cast<zCEventManager*>(obj);
				Release(obj);
			}
		}

		SetPhysicsEnabled((flags_pack2 >> 6) & 1);
	}
}
